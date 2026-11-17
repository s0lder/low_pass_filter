#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sndfile.h>

#define CHUNK_SIZE 4096
#define SUFFIX "-out"

int main(int argc, char **argv) {
	// ---------- processing cmdline args ---------
	if (argc < 3) {
		fprintf(stderr, "usage: %s INPUT_FILE_PATH LPF_BETA_VALUE\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *badchar;
	const double LPF_BETA = strtod(argv[2], &badchar);
	if (*badchar != '\0') {
		fprintf(stderr, "conversion failed, bad character %c in given LPF_BETA\n", *badchar);
		return EXIT_FAILURE;
	} else if (LPF_BETA <= 0 || LPF_BETA > 1) {
		fprintf(stderr, "LPF_BETA must be between 0 and 1\n");
		return EXIT_FAILURE;
	}

	char infile_path[PATH_MAX], outfile_name[FILENAME_MAX];

	char *last_slash = strrchr(argv[1], '/');
	if (strlen(argv[1]) >= PATH_MAX) {
		fprintf(stderr, "input path too long\n");
		return EXIT_FAILURE;
	}
	sprintf(infile_path, "%s", argv[1]);

	char *infile_basename = last_slash ? last_slash + 1 : infile_path;
	char *last_dot = strrchr(infile_basename, '.');
	if (last_dot != NULL && last_dot > infile_basename) {
		int dot_index = last_dot - infile_basename;
		if (dot_index + strlen(last_dot) + sizeof SUFFIX - 1 >= FILENAME_MAX) {
			fprintf(stderr, "input filename too long\n");
			return EXIT_FAILURE;
		}

		snprintf(outfile_name, dot_index + 1, "%s", infile_basename);
		snprintf(outfile_name + dot_index, sizeof SUFFIX + strlen(last_dot), "%s%s", SUFFIX, last_dot);
	} else {
		if (strlen(infile_basename) + sizeof SUFFIX - 1 >= FILENAME_MAX) {
			fprintf(stderr, "input filename too long\n");
			return EXIT_FAILURE;
		}
		if (snprintf(outfile_name, FILENAME_MAX, "%s%s", infile_basename, SUFFIX) < 0) {
			fprintf(stderr, "overflow!\n");
			return EXIT_FAILURE;
		}
	}

	// ---------- creating and opening input and output files ----------
	SNDFILE *infile, *outfile;

	struct SF_INFO sfinfo = { .format = 0 };

	if (!(infile = sf_open(infile_path, SFM_READ, &sfinfo))) {
		fprintf(stderr, "error opening file %s: %s\n", infile_path, sf_strerror(infile));
		return EXIT_FAILURE;
	}

	if (!(outfile = sf_open(outfile_name, SFM_WRITE, &sfinfo))) {
		fprintf(stderr, "error opening file %s: %s\n", outfile_name, sf_strerror(outfile));
		sf_close(infile);
		return EXIT_FAILURE;
	}

	// ---------- reading and writing ----------
	double input_buffer[CHUNK_SIZE];
	double output_buffer[CHUNK_SIZE] = {0};
	
	// size is the max channel count
	double prev_last_output[1024] = {0};

	int readcount;
	while ((readcount = (int) sf_read_double(infile, input_buffer, CHUNK_SIZE))) {
		for (int i = 0; i < readcount / sfinfo.channels; i++) {
			for (int channel = 0; channel < sfinfo.channels; channel++) {
				int idx = i * sfinfo.channels + channel;
				output_buffer[idx] = LPF_BETA * input_buffer[idx] + (1 - LPF_BETA) * prev_last_output[channel];
				prev_last_output[channel] = output_buffer[idx];
			}
		}
		sf_write_double(outfile, output_buffer, readcount);
	}

	sf_close(infile);
	sf_close(outfile);
}
