# low_pass_filter
Low-pass filter made in C.

Use `compile_and_run.sh` to compile and run the program effortlessly, like so:

`./compile_and_run.sh INPUT_FILE LPF_BETA_VALUE`

`INPUT_FILE` is a file in the current directory (not nested in any subdirectories), and `LPF_BETA_VALUE` is a double between 0 and 1. The smaller it is, the more "smoothing" the filter will do.

## Dependencies
The only external dependency is [`libsndfile`](https://github.com/libsndfile/libsndfile). You can either build it from source or install it using your package manager.

### Steps for installing it through your package manager:
1. Check if you have it installed: `pkg-config --modversion sndfile`
2. If not installed, I will list some of the more popular distros' packages that provide `libsndfile`:
    - Debian/Ubuntu: `sudo apt install libsndfile1-dev`
    - Fedora: `sudo dnf install libsndfile-devel`
    - Arch: `sudo pacman -S libsndfile`
