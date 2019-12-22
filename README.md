# ft_ls
An Unix Project at Codam (42) - ls command


## Evaluation criteria:

### Preliminary tests:

Check the following:
- repository is not empty
- repository doesn't contain unnecessarry or gabage files
- the author file is present
- the Makefile is present and contains mandatory rules
- the Makefile compiles the program in a proper way
- there are no Norminette errors or warnings

Did the work pass all the checks? If not, evaluation stops here.

`Yes` `No`

### Basic tests:

_The output must be identical to the original `ls`, excepting ACLs, extended attributes, and columns management._

Perform the following tests:
- ls
- ls -a
- ls -l
- Check the correct display of symbolic links.

Repeat all the checks without parameters, then with a file, then with a directory.

Did the work pass all the checks? If not, evaluation stops here.

`Yes` `No`

### Basic tests ++ :

Perform the following tests:
- ls -r
- ls -t
- test on devices, sockets (there are usually in / var / run /) and pipe names (man mkfifo).
- test with several files / directories as a parameter.
- test setuid / setgid / stickybit

Did the work pass all the checks? If not, evaluation stops here.

`Yes` `No`

### Less basic tests:

Perform the following tests:
- ls -R
- management of multiple options: parsing and 2 forms (eg "-l -t" but also "-lt")
- management of multiple options: correct display.

Did the work pass all the checks? If not, evaluation stops here.

`Yes` `No`

### Error handling:
- nonexistent file / directory
- inaccessible file / directory
- unmanaged or non-existent option

Did the work pass all the checks? If not, evaluation stops here.

`Yes` `No`