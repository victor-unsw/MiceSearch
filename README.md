# MiceSearch
search engine with small memory footprint

runtime memory limit : 12 MB


## How to compile :-
1. git clone https://github.com/victor-unsw/MiceSearch
2. Move to MiceSearch folder 
3. cmake .
4. make


## Syntax :-
```
./bazinga [input folder] [output file] 'keyword/phrase 1' ... 'keyword/phrase n'
```
*input folder*    : folder containing input files

*output file*     : file which will contain result

*keyword/phrase*  : search input, can be keyword or a phrase


## Features :-
1. **Single-pass-in-memory (SMPI)** used for index construction.

2. State of the art technique used for in memory **dictonary compression**.

3. Posting files compressed via **gamma encoding**.

4. Runtime memory stays below 12MB, irrespective to target text size.

5. Index construction consumes less than 20% of target text size over disk.
