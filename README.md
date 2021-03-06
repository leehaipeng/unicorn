# Unicorn

[![Build Status](https://travis-ci.org/vberlier/unicorn.svg?branch=master)](https://travis-ci.org/vberlier/unicorn)

> A magical unit testing framework for C.

Unicorn makes it easy to test C programs and libraries by providing a very simple and straight-forward API. The framework strives to reduce boilerplate code as much as possible to make the process of writing tests more enjoyable.

**:warning: Disclaimer :warning:**

This project is experimental. I'm relatively new to the C ecosystem, and I initially only developed Unicorn for personal use. The reason why I'm open-sourcing it is because it's actually been working pretty well for me so far.

```c
#include <unicorn/unicorn.h>

TEST(example)
{
    int product = 2 + 3;
    ASSERT_EQ(product, 6);
}

int main()
{
    return RUN_TESTS(example);
}
```

> Check out the [basic example](https://github.com/vberlier/unicorn/tree/master/examples/basic) for more details about this code snippet.

![Test output](examples/basic/test_output.png)

## Features

- Nested test groups
- Generic assertions
- Test parameters
- Test fixtures
- Capture test stdout and stderr
- Pretty output

## Installation

First, you'll need to download the source code.

```bash
$ git clone git://github.com/vberlier/unicorn.git
```

Now, compile the project with `make`.

```bash
$ make
```

Once the project is compiled, you're now ready to install Unicorn. The preferred way of installing it would be to install it on a per-project basis. Using the `DESTDIR` variable, you can specify where Unicorn should be installed.

```bash
$ make install DESTDIR=~/my_project
```

By using the command just above, `make` will copy the `libunicorn.so` library in `~/my_project/lib/` and the necessary headers in `~/my_project/include`.

You can also install Unicorn globally if you need to. The default destination is `/usr` so you'll need to run the command with the appropriate permissions.

```bash
$ sudo make install
```

## Uninstall

You can uninstall Unicorn using `make uninstall`. Just like in the installation process, use the `DESTDIR` variable to specify from where the Unicorn library and the related headers should be removed.

```bash
$ make uninstall DESTDIR=~/my_project
```

Once again, the default destination is `/usr` so you'll need to run the command with the appropriate permissions if you want to uninstall Unicorn globally.

```bash
$ sudo make uninstall
```

## Framework overview

### Defining tests

Unicorn lets you define tests using the `TEST` macro. The first argument is the name of the test. Note that the test name must be a valid identifier. The macro invocation should be followed by the test body, defined between curly braces.

```c
TEST(example)
{
    // Test body
}
```

The test body is simply a function body in which you can write your test.

As your test suite grows, you'll need to split your test definitions across multiple files to keep everything organized. You can use the `DECLARE_TEST` macro in order to declare tests in header files.

```c
// test_example.h

#ifndef TEST_EXAMPLE_H
#define TEST_EXAMPLE_H

#include <unicorn/unicorn.h>

DECLARE_TEST(example);

#endif
```

```c
// test_example.c

#include <unicorn/unicorn.h>

TEST(example)
{
    // Test body
}
```

### Grouping related tests together

You can define test groups for grouping related tests together with the `TEST_GROUP` macro. The first argument of the macro is the name of the test group. Note that the group name must be a valid identifier. The second argument of the macro must be an array literal where each element is either a test or a test group. This means that you can put test groups inside of other test groups.

```c
TEST(example1) { /* ... */ }
TEST(example2) { /* ... */ }

TEST_GROUP(example_group,
{
    example1,
    example2
})
```

In order to declare test groups inside of header files you'll need to use the `DECLARE_GROUP` macro.

```c
// test_example.h

#ifndef TEST_EXAMPLE_H
#define TEST_EXAMPLE_H

#include <unicorn/unicorn.h>

DECLARE_GROUP(example_group);

#endif
```

```c
// test_example.c

#include <unicorn/unicorn.h>

TEST(example1) { /* ... */ }
TEST(example2) { /* ... */ }

TEST_GROUP(example_group,
{
    example1,
    example2
})
```

### Running tests and test groups

Unicorn defines the `RUN_TESTS` macro. It will let you specify a list of tests and test groups to run and will return either `EXIT_SUCCESS` if all the tests passed or `EXIT_FAILURE` otherwise.

```c
TEST(foo) { /* ... */ }
TEST(bar) { /* ... */ }

TEST_GROUP(example_group, { /* ... */ })

int main()
{
    return RUN_TESTS(
        foo,
        bar,
        example_group
    );
}
```

Don't forget to include the headers in which you've declared the tests and test groups you want to run if you're defining your tests across multiple files.

```c
// run_tests.c

#include <unicorn/unicorn.h>

#include "test_foo.h"
#include "test_bar.h"
#include "test_example.h"

int main()
{
    return RUN_TESTS(
        foo,
        bar,
        example_group
    );
}
```

### Using assertions

Unicorn defines a few macros that are meant to be used inside of tests to report test failures. The most basic one is `FAIL`. It simply notifies Unicorn that the test failed and stops the test execution. You can optionally include an error message and use formatting to provide more details.

```c
TEST(example1)
{
    /* ... */

    if (result != 42)
    {
        FAIL();  // The test execution stops and Unicorn reports a failure
    }
}

TEST(example2)
{
    /* ... */

    if (result != 42)
    {
        FAIL("The result should be 42.");
    }
}

TEST(example3)
{
    /* ... */

    if (result != 42)
    {
        FAIL("The result should be 42 but got %d.", result);
    }
}
```

In practice, you might not want to use `FAIL` directly unless the way you determine that the test has failed doesn't rely on a meaningful expression. Most of the time, you'll actually use an assertion macro. The `ASSERT` macro essentially replaces the `if (!assertion) FAIL()` construct. The first argument of the macro should be the assertion that needs to be true for the test to succeed. If the assertion evaluates to false, Unicorn will report a failure and stop the test execution. You can also include an error message with formatting after the assertion.

```c
TEST(example1)
{
    /* ... */

    ASSERT(result == 42);
}

TEST(example2)
{
    /* ... */

    ASSERT(result == 42, "The result should be 42.");
}

TEST(example3)
{
    /* ... */

    ASSERT(result == 42, "The result should be 42 but got %d.", result);
}
```

If the assertion is a simple equality check, you can let Unicorn perform the comparison and format the error message for you by using the `ASSERT_EQ` macro. The macro is generic and works with most signed and unsigned integers of various sizes, floats and doubles. It can compare pointers and if the arguments are strings, it will check that they are identical using `strcmp`. Upon failure, Unicorn will display the values of both the expected and the actual result.

```c
TEST(example)
{
    /* ... */

    ASSERT_EQ(result, 42);
}
```

### Adding parameters to tests

It's quite common to want to check that a test passes with various different inputs. Instead of duplicating the test and only changing some hard-coded values, you can let Unicorn run your test several times with different inputs by using a test parameter. You can create a test parameter with the `TEST_PARAM` macro. The first argument of the macro is the name of the test parameter. It must be a valid identifer. The second argument is the type of the parameter. The last argument must be an array literal that contains all the values that you want the parameter to take.

```c
TEST_PARAM(input_number, int,
{
    0, -1, 8, 42
})
```

In order to apply a parameter to a test, you must include it in the list of test modifiers. The test modifiers are specified right after the test name in a test definition.

```c
TEST(example, input_number)
{
    // The test will run for each possible value of the parameter
}
```

If you specify multiple test parameters in the list of test modifiers, the test will run for every possible combination.

```c
TEST_PARAM(input1, char *, { "one", "two" })
TEST_PARAM(input2, int, { 1, 2, 3, 4 })

TEST(example, input1, input2)
{
    // The test will run with 8 different parameter combinations
}
```

In order to have access to the current value of a parameter inside of the test body, you'll need to use the `GET_PARAM` macro. The only argument of the macro is the name of the parameter that you want to bring in scope.

```c
TEST(example, input1, input2)
{
    GET_PARAM(input1);
    GET_PARAM(input2);

    // The current value of each parameter can now be used in the test
}
```

If you want to declare a test parameter inside of a header file, you'll need to use the `DECLARE_PARAM` macro. The first argument is the name of the parameter and the second one is the type.

```c
// input_number.h

#ifndef INPUT_NUMBER_H
#define INPUT_NUMBER_H

#include <unicorn/unicorn.h>

DECLARE_PARAM(input_number, int);

#endif
```

```c
// input_number.c

#include <unicorn/unicorn.h>

TEST_PARAM(input_number, int,
{
    0, -1, 8, 42
})
```

### Using test fixtures

Test fixtures let you provide data to a test from the outside. This allows you to keep the body of the test focused on making assertions instead of executing setup and teardown code. By extracting cleanup instructions outside of the test, you can be sure that they will be executed even if the test fails. In order to define a test fixture, you'll need to use the `TEST_FIXTURE` macro. The first argument of the macro is the name of the fixture. The name must be a valid identifier. The second argument is the type of the fixture. The macro invocation must be followed by the fixture body, defined between two curly braces.

```c
TEST_FIXTURE(number, int)
{
    *number = 42;
}
```

The fixture body is simply a function body in which you can set the value of the fixture. Unicorn allocates the necessary memory according to the type you specified. You can then initialize this memory with the provided pointer.

You can additionally specify cleanup instructions using the `CLEANUP_FIXTURE` macro. It must be invoked at the end of the fixture body and the first argument must be the name of the fixture. The invocation should be followed by a code block in which you'll be able to put your cleanup code.

```c
TEST_FIXTURE(message, char *)
{
    char value[] = "Hello, World!";
    *message = malloc(sizeof (value));
    strncpy(*message, value, sizeof (value));

    CLEANUP_FIXTURE(message)
    {
        free(*message);
    }
}
```

In order to apply a fixture to a particular test, you'll need to specify the name of the fixture inside of the list of test modifiers.

```c
TEST(example, message)
{
    // Unicorn will execute the necessary setup and teardown code
}
```

To have access to the value of the fixture, you'll need to use the `GET_FIXTURE` macro. The only argument of the macro is the name of the fixture that you want to bring in scope.

```c
TEST(example, message)
{
    GET_FIXTURE(message);

    // The value of the fixture can now be used in the test
}
```

If you want to declare a test fixture inside of a header file, you'll need to use the `DECLARE_FIXTURE` macro. The first argument is the name of the fixture and the second one is the type.

```c
// number.h

#ifndef NUMBER_H
#define NUMBER_H

#include <unicorn/unicorn.h>

DECLARE_FIXTURE(number, int);

#endif
```

```c
// number.c

#include <unicorn/unicorn.h>

TEST_FIXTURE(number, int)
{
    *number = 42;
}
```

You can check out the [tmpdir fixture example](https://github.com/vberlier/unicorn/tree/master/examples/tmpdir_fixture) if you want to see an example of a more practical fixture.

Most of the features that are available with tests are also available with fixtures. You can use assertions and apply test modifiers. Assertions allow you to cleanly exit a test if there's a problem during setup or teardown code. If the setup code fails for instance, Unicorn will report the error right away without attempting to execute the test.

```c
TEST_FIXTURE(text_file, FILE *)
{
    *text_file = fopen("foo.txt", "r");
    ASSERT(*text_file != NULL, "Couldn't open \"foo.txt\".");

    CLEANUP_FIXTURE(text_file)
    {
        fclose(*text_file);
    }
}
```

You can apply test modifiers by including them in the list of modifiers right after the type of the fixture. You'll need to use the `GET_PARAM` and `GET_FIXTURE` macros if you want to access the value of a modifier inside of the fixture body.

```c
TEST_PARAM(filename, char *, { "foo.txt", "bar.txt" })

TEST_FIXTURE(text_file, FILE *, filename)
{
    GET_PARAM(filename);

    *text_file = fopen(filename, "r");
    ASSERT(*text_file != NULL, "Couldn't open \"%s\".", filename);

    CLEANUP_FIXTURE(text_file)
    {
        fclose(*text_file);
    }
}
```

Note that when a fixture with modifiers is applied to a test, all the modifiers are registered on the test itself. For each test, Unicorn recursively resolves all the parameters and fixtures that are being used and applies them directly to the test. If several fixtures all require a particular modifier, they will share the same instance.

### Managing test resources

Unicorn can take care of freeing memory for you at the end of a test. You can register a pointer to be automatically freed by using the `auto_free()` function. This allows you to eliminate calls to `free()` from the end of your tests and ensures that no matter the outcome of the test, the allocated memory is always released.

```c
TEST(example)
{
    char *buffer = malloc(256);
    auto_free(buffer);

    // The allocated memory will be released at the end of the test
}
```

You can also use the `test_resource()` function instead of `malloc()` if you don't want to worry about calling `auto_free()` after allocating the memory.

```c
TEST(example)
{
    char *buffer = test_resource(256);

    // The allocated memory will be released at the end of the test
}
```

It's worth mentioning that letting Unicorn release memory for you can often eliminate the need for cleanup code in fixtures.

## Contributing

Contributions are welcome. Feel free to open an issue if you're having troubles or if you want to suggest some improvements.

The test suite for Unicorn is built with Unicorn. It's currently far from complete but you can run it with `make test`. If you set the `DEBUG` variable to `1`, the test executable will be compiled with AddressSanitizer enabled.

```bash
$ make test DEBUG=1
```

---

License - [MIT](https://github.com/vberlier/unicorn/blob/master/LICENSE)
