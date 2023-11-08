# Overview

Given a pattern, generate all possible combinations.

Pattern Expander was intended as password recovery tool where the 
password follows a certain pattern. 

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


Characters between [ ] are a group. Every character in a group is substituted
at the given position.

## Installation

### Requirements
- gcc
- make
- gtest
- gcov

On Debian, those can be installed via:
`sudo apt-get install build-essential libgtest-dev gcovr`

### Build and install
```sh
make
sudo make install
```

You don't need to install patext, after building, you can run it from the the `./bin/` folder

## Usage
```
patexp [-h | --help] <command> [<args>]
		commands:
			run [<pat1> <pat2> .. ] | <stdin> : Generates strings from a list of patterns
			validate [<pat1> <pat2> .. ] | <stdin> : Validates if the patterns provided are syntaxically correct
			configure: Sets the various configuration options
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
`abc[123]` - abc is static part. That part of the pattern doesn't change.

### Groups:
A group is a group of characters that are possible
- [abc] - Substitubes one each of the listed symbols 
- [a - z] - Range. Expands to be equivalend to [a...z]
- \ : Escape character

All characters are case sensative.


## Examples:
### Static examples
	Pattern: "abcd"
	Result: "abcd"
	
	Pattern: "abcd//e"
	Result: "abcd/e"
	
	Pattern: "abcd/-"
	Result:  "abcd-"
	
	
### Group examples
	Pattern: "[abc]"
	Result: "a", "b", "c"

	Pattern: [a-c]
	Result: "a", "b", "c"
	
	Pattern: [c-a]
	Result: "c", "b", "a"
	
	Pattern: [A-C]
	Result: "A", "B", "C"

	Pattern: [123][abc]
	Result: 1a, 2a, 3a, 1b, 2b, 3b, 1c, 2c, 3c

	Pattern: [1-3][a-c]
	Result: 1a, 2a, 3a, 1b, 2b, 3b, 1c, 2c, 3c
	
	Pattern: [123[a-c]]
	Result: 1a, 2a, 3a, 1b, 2b, 3b, 1c, 2c, 3c


## Development

### Targets:
- release: default target. Builds the main executable
- debug:   Builds with debug information and test coverage enabled
- test:    Builds with debug information and test coverage enabled. Builds and runs the unit tests.
- report:  Creates a test coverage report


### Testing
Some example patters are given in the `test_data` folder. Those can be used for system level testing.
For example:

```sh
cat test_data/valid_patterns.txt | patext run 
```

```sh
cat test_data/valid_patterns.txt | xargs patext run 
```

### Run unit tests
`make test`


