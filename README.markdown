# ruby_php #

An *early one-evening prototype* of a project to embed PHP inside Ruby so that legacy PHP code can be called from shiny new Ruby code. The goal is to not only be able to call PHP functions and methods but also wrap objects, arrays and blocks so that they can be passed between languages with relatively few surprises. Possible use cases include craziness like using RSpec to test a Zend Framework project or using ActiveRecords from a PHP script.

I'll probably really start this project during 2012.

## Outlook ##

Basic functionality seems easy, but lots of corner cases are to be expected before this can reach production quality. Specific issues that need to be handled include:

- Array, object and resource wrappers. PHP's copy-on-write scheme may present subtleties.
- Exception handling when the call stack has PHP and Ruby calls intertwined.
- PHP error handling. Probably can't do much about fatal PHP errors.
- Procs as anonymous functions and vice-versa.
- Memory management, which involves e.g. PHP references and garbage collection must be written carefully. Ruby (MRI at least) has a pretty simple conservative collector and PHP has reference counting.
- PHP 5.3 namespaces
- PHP 5.4 traits
- Syncing of web features like header(), $_SERVER, $_GET, $_POST and $_COOKIE.

## Links ##

* ["Embedding PHP" - presentation by Sara Golemon](http://www.docstoc.com/docs/10783691/Embedding-PHP)
* ["Embedding the PHP Interpreter" - blog post by Aivars Kalvāns](http://phi.lv/?p=376)
* [README.EXT of MRI - readme for Ruby extension writers](https://github.com/ruby/ruby/blob/trunk/README.EXT)

## LICENSE ##

None. Public domain.

