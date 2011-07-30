
#include <assert.h>
#include <ruby.h>
#include <zend.h>
#include <zend_exceptions.h>
#include <php.h>
#include <php/sapi/embed/php_embed.h>

#if __GNUC__ >= 4
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)
#else
#define likely(x)    (!!(x))
#define unlikely(x)  (!!(x))
#endif


VALUE php_module;
static VALUE PHPError;
static VALUE PHPConversionError;
static VALUE PHPException; // will eventually be the same as PHP.Exception, a wrapper for PHP's Exception


void Init_ruby_php();

static void check_php_still_alive();
static void raise_current_php_exception_in_ruby();

static void define_php_module_method(const char* name, VALUE (*func)(), int argc);

static VALUE eval_php_stmt(VALUE mod, VALUE code);
static VALUE eval_php_expr(VALUE mod, VALUE code);

static VALUE phpval_to_ruby_and_dtor(zval* zv);


/* A begin_php/end_php block will catch PHP exceptions and fatal errors
   and rethrow them as Ruby exceptions. */
//TODO: ensure (that is, unit test) that zend_first_try is safe when ruby calls php which calls ruby which calls php
#define begin_php { \
        int fatal_error_in_php = 0;\
        zend_first_try {
#define end_php \
        } zend_catch { \
            fatal_error_in_php = 1; \
        } zend_end_try() \
        if (fatal_error_in_php) { \
            rb_raise(PHPError, "PHP fatal error"); \
        } \
        if (EG(exception)) { \
            raise_current_php_exception_in_ruby(); \
        } \
    }

void Init_ruby_php()
{
    static char* argv[] = {"ruby_php", NULL};
    if (php_embed_init(1, argv PTSRMLS_CC) == FAILURE) {
        rb_raise(rb_eLoadError, "Failed to initialize PHP");
    }
    
    php_module = rb_define_module("PHP");
    PHPError = rb_define_class("PHPError", rb_eScriptError);
    PHPConversionError = rb_define_class("PHPConversionError", PHPError);
    PHPException = rb_define_class("PHPException", rb_eStandardError);
    
    define_php_module_method("eval_stmt", eval_php_stmt, 1);
    define_php_module_method("eval_expr", eval_php_expr, 1);
}

static void check_php_still_alive()
{
    if (CG(unclean_shutdown)) { // i.e. zend_bailout has been called
        // I'll have to check some day but I think the following is the case.
        rb_raise(PHPError, "PHP has suffered a fatal exception and is no longer safe to use in this process. PHP memory and resources may also be leaked.");
    }
}

static void raise_current_php_exception_in_ruby()
{
    assert(EG(exception));
    
    if (EG(prev_exception)) {
        zval_ptr_dtor(&EG(prev_exception));
        EG(prev_exception) = NULL;
    }
    
    //TODO: do something like in zend_exception_error and
    //      throw a wrapper exception class
    
    zval_ptr_dtor(&EG(exception));
    EG(exception) = NULL;
    rb_raise(PHPException, "PHP exception [TODO: details]");
}

static void define_php_module_method(const char* name, VALUE (*func)(), int argc)
{
    rb_define_method(php_module, name, func, argc);
}

static VALUE eval_php_stmt(VALUE mod, VALUE code)
{
    check_php_still_alive();

    Check_Type(code, T_STRING);
    char* code_cstr = StringValueCStr(code);
    
    begin_php {
        if (zend_eval_string_ex(code_cstr, NULL, "eval_stmt() from Ruby", 0) == FAILURE) {
            zend_throw_exception(NULL, "eval_stmt() from Ruby failed", 0);
        }
    } end_php
    
    return Qnil;
}

static VALUE eval_php_expr(VALUE mod, VALUE code)
{
    check_php_still_alive();

    Check_Type(code, T_STRING);
    char* code_cstr = StringValueCStr(code);
    zval retval;
    
    begin_php {
        if (zend_eval_string_ex(code_cstr, &retval, "eval_expr() from Ruby", 1) == FAILURE) {
            zend_throw_exception(NULL, "eval_stmt() from Ruby failed", 0);
        }
        if (EG(exception)) {
            zval_dtor(&retval);
        }
    } end_php
    
    return phpval_to_ruby_and_dtor(&retval);
}

static VALUE phpval_to_ruby_and_dtor(zval* val)
{
    VALUE result;
    const char* error = NULL;
    
    switch (Z_TYPE_P(val)) {
    case IS_NULL:
        result = Qnil;
        break;
    case IS_BOOL:
        result = Z_BVAL_P(val) ? Qtrue : Qfalse;
        break;
    case IS_LONG:
        result = LONG2NUM(Z_LVAL_P(val));
        break;
    case IS_RESOURCE:
        error = "TODO: wrap PHP resource";
        break;
    case IS_DOUBLE:
        result = rb_float_new(Z_DVAL_P(val));
        break;
    case IS_STRING:
        result = rb_str_new(Z_STRVAL_P(val), Z_STRLEN_P(val));
        break;
    case IS_ARRAY:
        error = "TODO: wrap PHP array";
        break;
    case IS_OBJECT:
        error = "TODO: wrap PHP object";
        break;
    default:
        error = "Unknown PHP datatype";
        break;
    }
    
    zval_dtor(val);
    
    if (likely(!error)) {
        return result;
    } else {
        rb_raise(PHPConversionError, error);
    }
}

