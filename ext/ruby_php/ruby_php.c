
#include <ruby.h>

void Init_ruby_php();
static VALUE hello_world(VALUE mod);

void Init_ruby_php()
{
  VALUE module = rb_define_module("PHP");
  rb_define_method(module, "hello_world", hello_world, 0);
}

static VALUE hello_world(VALUE mod)
{
  return rb_str_new2("hello world");
}

