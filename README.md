# Overview

Given a pattern, generate all possible combinations.

Pattern Expander was intended as password recovery tool where the 
password follows a certain pattern. 

For example `p[a@][s$][s$]` will result in:
`pass, p@ss, pa$s, p@$s, pas$, p@s$, pa$$, p@$$`

Characters between [ ] are a group. Every character in a group is substituted
at the given position.


##Syntax

###Static strings
Any unadorned strings are considered static. For example given the pattern
`abc[123]` - abc is static part. That part of the pattern doesn't change.

###Groups:
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


## Building

### Targets:
- release: default target. Builds the main executable
- test: Builds the main executable with debug info. Builds and runs the unit tests.
- report - creates a coverage report


### Install build requirements:
`sudo apt-get install build-essential libgtest-dev gcovr`

### Compile:
`make`

###Run unit tests
`make test`


## Usage
`patexp <pattern>`

