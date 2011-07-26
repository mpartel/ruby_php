
#include <ruby.h>
#include <php.h>
#include <php/sapi/embed/php_embed.h>

void Init_ruby_php();
static VALUE eval_php_stmt(VALUE mod, VALUE code);
static VALUE eval_php_expr(VALUE mod, VALUE code);
static VALUE phpval_to_ruby(zval* zv);
static void raise_error_from_php(const char* msg);

void Init_ruby_php()
{
    VALUE module = rb_define_module("PHP");
    
    static char* argv[] = {"ruby_php", NULL};
    if (php_embed_init(1, argv PTSRMLS_CC) == FAILURE) {
        rb_raise(rb_eRuntimeError, "Failed to initialize PHP");
    }
    
    rb_define_method(module, "eval", eval_php_stmt, 1);
    rb_define_method(module, "eval_expr", eval_php_expr, 1);
}

static VALUE eval_php_stmt(VALUE mod, VALUE code)
{
    Check_Type(code, T_STRING);
    char* code_cstr = StringValueCStr(code);
    int error = 0;
    zend_first_try {
        if (zend_eval_string_ex(code_cstr, NULL, "eval() from Ruby", 1) == FAILURE) {
            error = 1;
        }
    } zend_catch {
        error = 2;
    } zend_end_try();
    
    return Qnil;
}

static VALUE eval_php_expr(VALUE mod, VALUE code)
{
    Check_Type(code, T_STRING);
    char* code_cstr = StringValueCStr(code);
    zval retval;
    int error = 0;
    zend_first_try {
        if (zend_eval_string_ex(code_cstr, &retval, "eval() from Ruby", 1) == FAILURE) {
            error = 1;
        }
    } zend_catch {
        error = 2;
    } zend_end_try();
    
    VALUE retval_ruby;
    switch (error) {
        case 1:
            zval_dtor(&retval);
            raise_error_from_php("PHP syntax error");
        case 2:
            zval_dtor(&retval);
            raise_error_from_php("Got a PHP error");
        default:
            retval_ruby = phpval_to_ruby(&retval); //FIXME: may raise ruby error
            return retval_ruby;
    }
}

static VALUE phpval_to_ruby(zval* val)
{
    switch (Z_TYPE_P(val)) {
        case IS_NULL:
            return Qnil;
        case IS_BOOL:
          return Z_BVAL_P(val) ? Qtrue : Qfalse;
        case IS_LONG:
          return LONG2NUM(Z_LVAL_P(val));
        case IS_RESOURCE:
            raise_error_from_php("TODO: wrap PHP resource");
            break;
        case IS_DOUBLE:
            return rb_float_new(Z_DVAL_P(val));
        case IS_STRING:
            return rb_str_new(Z_STRVAL_P(val), Z_STRELN_P(val));
        case IS_ARRAY:
            raise_error_from_php("TODO: wrap PHP array");
            break;
        case IS_OBJECT:
            raise_error_from_php("TODO: wrap PHP object");
            break;
        default:
            raise_error_from_php("Unknown PHP datatype");
            break;
    }
}

static void raise_error_from_php(const char* msg)
{
    rb_raise(rb_eRuntimeError, msg);
}

