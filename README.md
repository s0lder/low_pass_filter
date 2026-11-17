# low_pass_filter
Low-pass filter made in C.

Use `compile_and_run.sh` to compile and run the program effortlessly, like so:
`./compile_and_run.sh INPUT_FILE LPF_BETA_VALUE`

`INPUT_FILE` is a file in the current directory (not nested in any subdirectories), and `LPF_BETA_VALUE` is a double between 0 and 1. The smaller it is, the more "smoothing" the filter will do.
