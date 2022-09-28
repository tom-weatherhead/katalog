# katalog

(I.e. A Japanese 'kata' plus 'log' for Prolog)

A small Prolog interpreter

[![watchers][watchers-badge-image]][watchers-url]
[![stars][stars-badge-image]][stars-url]
[![issues][issues-badge-image]][issues-url]
[![forks][forks-badge-image]][forks-url]
[![contributors][contributors-badge-image]][contributors-url]
[![branches][branches-badge-image]][branches-url]
[![releases][releases-badge-image]][releases-url]
[![commits][commits-badge-image]][commits-url]
[![last commit][last-commit-badge-image]][last-commit-url]
[![license][license-badge-image]][license-url]

## To compile and run katalog

First, ensure that gcc and make are installed.

After cloning this repository, cd into its directory and do this:

```sh
$ cd src
$ make
$ ./katalog
```

From the src directory, you can do the following:

To do a full rebuild and run the built-in tests:

```sh
$ make clean && make && make test
```

To run a script: E.g.:

```sh
$ ./katalog ../scripts/test001.pro
```

To enter the read-eval-print loop (REPL) :

```sh
$ ./katalog
```

Exit the REPL by typing 'exit', 'quit', or 'bye'.

## License
[MIT](https://choosealicense.com/licenses/mit/)

[watchers-badge-image]: https://badgen.net/github/watchers/tom-weatherhead/katalog
[watchers-url]: https://github.com/tom-weatherhead/katalog/watchers
[stars-badge-image]: https://badgen.net/github/stars/tom-weatherhead/katalog
[stars-url]: https://github.com/tom-weatherhead/katalog/stargazers
[issues-badge-image]: https://badgen.net/github/issues/tom-weatherhead/katalog
[issues-url]: https://github.com/tom-weatherhead/katalog/issues
[forks-badge-image]: https://badgen.net/github/forks/tom-weatherhead/katalog
[forks-url]: https://github.com/tom-weatherhead/katalog/network/members
[contributors-badge-image]: https://badgen.net/github/contributors/tom-weatherhead/katalog
[contributors-url]: https://github.com/tom-weatherhead/katalog/graphs/contributors
[branches-badge-image]: https://badgen.net/github/branches/tom-weatherhead/katalog
[branches-url]: https://github.com/tom-weatherhead/katalog/branches
[releases-badge-image]: https://badgen.net/github/releases/tom-weatherhead/katalog
[releases-url]: https://github.com/tom-weatherhead/katalog/releases
[commits-badge-image]: https://badgen.net/github/commits/tom-weatherhead/katalog
[commits-url]: https://github.com/tom-weatherhead/katalog/commits/master
[last-commit-badge-image]: https://badgen.net/github/last-commit/tom-weatherhead/katalog
[last-commit-url]: https://github.com/tom-weatherhead/katalog
[license-badge-image]: https://img.shields.io/github/license/mashape/apistatus.svg
[license-url]: https://github.com/tom-weatherhead/thaw-grammar/blob/master/LICENSE
