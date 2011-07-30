# ruby_php #

An *early two-evening prototype* of a project to embed PHP inside Ruby so that legacy PHP code can be called from shiny new Ruby code. The goal is to not only be able to call PHP functions and methods but also wrap objects, arrays and blocks so that they can be passed between languages with relatively few surprises. Possible use cases include craziness like using RSpec to test a Zend Framework project or using ActiveRecords from a PHP script.

I'll probably really start this project during 2012.


## What already works ##

* Evaluating PHP code ([test case](https://github.com/mpartel/ruby_php/blob/master/test/test_eval.rb))
* Converting strings, ints and booleans back to Ruby.
* Catching PHP exceptions and raising them as Ruby exceptions.


## TODO ##

- Converting values
    - Wrappers for arrays, objects and resources.
    - In Ruby, everything is an object, while PHP has a less simple copy-on-write scheme for arrays and strings.
    - `ArrayObject` is a possibility, but it integrates poorly with PHP's existing array functions.
    - Ruby Bignums will probably have to be converted to PHP floats.
    - Procs should be converted to anonymous functions and vice versa.

- Converting classes and functions
    - Registering Ruby procs as PHP functions.
    - Exporting Ruby classes to PHP and vice versa.
    - Implementing PHP interfaces with Ruby classes.
    - Calling PHP functions magically from Ruby. E.g. `PHP.explode(" ", "ruby string")` => `["ruby", "string"]`.
    - Possibly convert PHP 5.3 namespaces into Ruby modules.
    - Possibly convert PHP 5.4 traits into Ruby modules.

- Error handling
    - Conversion of exception objects back and forth when the call stack has PHP and Ruby calls intertwined. Ruby uses longjmp for raising exceptions while PHP sets a global exception pointer that the interpreter looks for and uses longjmp for fatal errors instead.
    - I'm unsure whether the PHP in a process can be safely reused after a fatal error i.e. a `zend_bailout`.

- Memory management
    - Valgrind helps catch some errors but GC complicates things.
    - Ruby (MRI at least) has a simple conservative stack-scanning collector and PHP has simple reference counting with an optional collector.
    - Wrapped Ruby pointers that go off stack will probably have to be marked as GC roots. Not sure yet whether PHP's GC scans the C stack.

- Miscellaneous
    - Web features like header(), $_SERVER, $_GET, $_POST and $_COOKIE should be synced. The SAPI infrastructure should provide for this.
    - Building as a PHP extension to embed a Ruby interpreter inside PHP.


## Building ##

* Install dependencies:

```bash
gem install rake --version '=0.8.7'
gem install hoe rake-compiler
```

* Compile PHP with `libphp5.so` (i.e. the 'embed' SAPI)

```bash
cd php-5.x.y
./configure --prefix=$HOME/opt/php-embed --enable-embed
make
make install
```

* Compile and run the tests

```bash
rake test
```

or with [Valgrind](http://valgrind.org/)

```bash
rake test:valgrind
```

## Links ##

* ["Embedding PHP" - presentation by Sara Golemon](http://www.docstoc.com/docs/10783691/Embedding-PHP)
* ["Embedding the PHP Interpreter" - blog post by Aivars Kalvāns](http://phi.lv/?p=376)
* [README.EXT of MRI - readme for Ruby extension writers](https://github.com/ruby/ruby/blob/trunk/README.EXT)

## LICENSE ##

None. Public domain.

