# Overview

Given a pattern, generate all possible combinations.

Pattern Expander was intended as password recovery tool where the password follows a certain pattern.
It uses UTF-8 and has been tested with Cyrillic, but it shoud be able to support other alphabets as well.

For example `p[a@][s$][s$]` will result in:

```sh
pass
p@ss
pa$s
p@$s
pas$
p@s$
pa$$
p@$$
```


## Installation

There are no binary distributions at this time, you have to compile it from source.

### Requirements
- gcc
- make
- gtest
- gcov

On Debian, those can be installed via:
`sudo apt-get install build-essential libgtest-dev gcovr`

### Build and install
Clone the repo, cd into the source directory and run the following

```sh
make
```

You don't need to install patext, after building, you can run it from the the `./bin/` folder
but if you want it to be in the path `sudo make install` will copy it to `/usr/local/bin`

## Usage
```
patexp [-h | --help] <command> [<args>]
		commands:
			run [<pat1> <pat2> .. ] | <stdin> : Generates strings from a list of patterns
			validate [<pat1> <pat2> .. ] | <stdin> : Validates if the patterns provided are syntaxically correct
			configure: Sets the various configuration options <Not yet implemented>
```

### Examples

Assuming you have compiled and running out of the local bin folder:

```sh
$./bin/patexp run [a-c] [1-3] abc[1-3]
a
b
c
1
2
3
abc1
abc2
abc3

```

We can load the patterns from a file via

```sh
$cat patters.txt | ./bin/patexp run
a
b
c
1
2
3
abc1
abc2
abc3

```


## Syntax

### Static strings
Any unadorned strings are considered static. For example given the pattern
`abc[123]` - `abc` is static part. That part of the pattern doesn't change.

#### Example
	Pattern: "abcd"
	Result: "abcd"

### Groups:
A group is a group of characters that are possible  at the given position
- [abc] - Substitubes one each of the listed symbols at the given position
- [a-z] - Range. Expands to be equivalend to [a...z] works for numeric values as well ( [0-9] only )
- [z-a] - Range. Expands to be equivalend to [z...a] works for numeric values as well ( [0-9] only )


#### Examples
	Pattern: "[abc]"
	Result: a, b, c

	Pattern: [a-c]
	Result: a, b, c
	
	Pattern: [c-a]
	Result: c, b, a
	
	Pattern: [A-C]
	Result: A, B, C

	Pattern: [123][abc]
	Result: 1a, 2a, 3a, 1b, 2b, 3b, 1c, 2c, 3c

	Pattern: [1-3][a-c]
	Result: 1a, 2a, 3a, 1b, 2b, 3b, 1c, 2c, 3c
	
	Pattern: [123[a-c]]
	Result: 1a, 2a, 3a, 1b, 2b, 3b, 1c, 2c, 3c
	
### Escape character
To include special characters such as `[ ]` and `-` in your pattern you have to escape them. This is done with the `/` character

	Pattern: "abcd/[e"
	Result: "abcd[e"
	
	Pattern: "abcd/]"
	Result:  "abcd]"
	
You do not need to escape the range (`-`) character in static portions of the stiring. For example:

	Pattern: "a-bcd"
	Result:  "a-bcd"
	
	Pattern: "a/bcd"
	Result:  "a/bcd"
	
You do need to escape the escape character (`/`) in the next character is a reserved character. For example:

	Pattern: a//[1-3]
	Result:  a/1, a/2, a/3
	
Inside groups, you need to escape the range (`-`) symbol:

	Pattern: [a/-b]
	Result:  a, -, b
	

### Quotes
Quotes are used to denote static sections. They can be useful when you have a section that contains a lot of special characters.
Be aware that when running from the shell, you'll need to escape those. Using single quotes around your patters works well.

	Pattern: "[a-c]"
	Result:  [a-c]
	
You can escape quotes in the usual fashion:

	Pattern: /"[a-b]/"
	Result:  "a", "b", "c"


## Development

### Targets:
- release: default target. Builds the main executable
- debug:   Builds the main executable with debug information and test coverage enabled
- test:    Builds the main executable with debug information and test coverage enabled. Builds and runs the unit tests.
- report:  Creates a test coverage report


### Testing
Some example patters are given in the `test_data` folder. Those can be used for system level testing.
For example:

```sh
cat test_data/valid_patterns.txt | ./bin/patext run 
```

```sh
cat test_data/valid_patterns.txt | xargs ./bin/patext run 
```

### Run unit tests
`make test`


