# 15puzzle

This is a game for playing the famous 15 puzzle game. Be the champion and conquer the grid!

## Building

To run the game clone this repository and run the following commands:

```bash
gcc -O3 game.c image.c main.c -o img -lm
```

## Usage

```
❯ ./img -h
USAGE:
    ./img [-i<input>] [-o<output>] [-r<output>] [-N<width>] [-M<height>] [-h] 

OPTIONS:
    -i, --input <input>         Input ppm file (default: in.ppm)
    -o, --output-file <output>  Output ppm file (default: out.ppm)
    -r, --output-root <output>  Output prefix of ppm file (default: )
    -N, --width <width>         Grid width (default: 4)
    -M, --height <height>       Grid height (default: 4)
    -h, --help
```

To play the game simply pass the input file you want to play on (the supported types are pmm ascii
and ppm binary). You can also try one of the test files.

```bash
./img -itests/15-puzzle.binary.ppm
```

Press <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd> to move the empty tile. The input is
instantaneous meaning you don't have to press space (at least on unix-like systems). Try to move the
randomised tiles so that they create the input image.

You need to use a previewer that supports auto reload. Here's an example command using `feh`.

```bash
feh --auto-reload -ZFY out.ppm
```

## Preview

<div align="center">
  <video
    width="80%"
    src="https://github.com/user-attachments/assets/8814542d-8633-481b-862a-e7a40c4259e7"
    poster="https://github.com/user-attachments/assets/d553cb9e-254c-4dde-b1ff-4ff098954582"
  >
  </video>
</div>
