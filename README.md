# Overview

Given a pattern, generate all possible combinations.

Pattern Expander was intended as tool to help password recovery where the password follows a certain pattern.
The user would provide a pattern and PatternExpander would generate all possible passwords. Those can then be used to brute force the password.
 

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

It uses UTF-8 and has been tested with Cyrillic, but it shoud be able to support other alphabets as well.


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

You don't need to install PatternExpander, after building, you can run it from the the `./bin/` folder
but if you want it to be in the path `sudo make install` will copy it to `/usr/local/bin`

## Usage
```
patexp [-h | --help] <command> [<args>]
		commands:
			run [<pat1> <pat2> .. ] | <stdin> : Generates strings from a list of patterns
			validate [<pat1> <pat2> .. ] | <stdin> : Validates if the patterns provided are syntaxically correct
			configure: Sets the various configuration options
				[-b, --group_begin <val>] : Sets the group begin symbol. Default: '['
				[-e, --esc         <val>] : Sets the escape symbol. Default: '/'
				[-q, --quote       <val>] : Sets the quote symbols. Default: '"'
				[-n, --group_end   <val>] : Sets the group end symbol. Default: ']'
				[-r, --range       <val>] : Sets the range symbol. Default: '-'
```


### Run

Run loops over the provided patters and generates all possible outputs for each. 
Generally, invalid patterns are skipped and PatternExpander proceeds with the next pattern.
The Run sub-command aceepts patters as arguments or via stdin. Patterns are space delimited.
If you have to have spaces or (other special characters such as quotes) in your patterns, you need to make sure they are properly escaped.

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

For more examples see the Syntax section.

### Validate

Similarly to run, validate loops over the provided patters and generates does basic validation for each.
Errors are outputed to stderr. 
The Validate sub-command aceepts patters as arguments or via stdin. Patterns are space delimited.
If you have to have spaces or (other special characters such as quotes) in your patterns, you need to make sure they are properly escaped.

Assuming you have compiled and running out of the local bin folder:

```sh
$/bin/patexpd validate ]]a-c
Error: Invalid group sequence detected
]]a-c
^
```

### Configure

Configure is used to set the various reserved characters used by PatternExpander.
Configure stores those value in `$HOME/.patexpconfig`
If that file exists, the values are loaded from it. If it does not, the default values are assumed (see Usage for more details).
All configuration settings are optional - if not provided the existing setting will remain unchanged.

Note: Calling the configure sub-command overrides (or creates) the .patexpconfig file.


## Syntax

### Static strings
Any unadorned strings are considered static. For example given the pattern
`abc[123]` - `abc` is static part. That part of the pattern doesn't change.

#### Example
	Pattern: "abcd"
	Result: "abcd"

### Groups:
A group is a group of characters that are possible at the given position
- [abc] - Substitubes one each of the listed symbols at the given position
- [a-z] - Range. Expands to be equivalend to [a...z] works for numeric values as well ( [0-9] only )
- [z-a] - Range. Expands to be equivalend to [z...a] works for numeric values as well ( [0-9] only )


#### Examples
For brevity, the resuls are shown as comma separated values, but in reality there will be line ending after each (and no commas)

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


