#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#define CHUNK_SIZE 4096
#define MAX_FILENAME_LEN 256
#define SUFFIX "out-"

int main(int argc, char **argv) {
	// ---------- processing cmdline args ---------
	if (argc < 3) {
		fprintf(stderr, "usage: %s INPUT_FILE LPF_BETA_VALUE\n", argv[0]);
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

	char infile_name[MAX_FILENAME_LEN], outfile_name[MAX_FILENAME_LEN];
	
	if (strlen(argv[1]) >= MAX_FILENAME_LEN) {
		fprintf(stderr, "input filename too long");
		return EXIT_FAILURE;
	}

	sprintf(infile_name, "%s", argv[1]);

	size_t dot_index = strlen(infile_name) - 1;
	while (infile_name[dot_index] != '.' && dot_index > 0)
		dot_index--;

	if (dot_index > 0) {
		snprintf(outfile_name, dot_index + 1, "%s", infile_name);
		if (strlen(outfile_name) + strlen(infile_name + dot_index) + sizeof SUFFIX - 1 >= MAX_FILENAME_LEN) {
			fprintf(stderr, "input filename too long");
			return EXIT_FAILURE;
		}
		strcat(strcat(outfile_name, "-out"), infile_name + dot_index);
	} else {
		if (strlen(infile_name) + sizeof SUFFIX >= MAX_FILENAME_LEN) {
			fprintf(stderr, "input filename too long");
			return EXIT_FAILURE;
		}
		sprintf(outfile_name, "%s", infile_name);
		strcat(outfile_name, "-out");
	}

	// ---------- creating and opening input and output files ----------
	SNDFILE *infile, *outfile;

	struct SF_INFO sfinfo = { .format = 0 };

	if (!(infile = sf_open(infile_name, SFM_READ, &sfinfo))) {
		fprintf(stderr, "error opening file %s: %s\n", infile_name, sf_strerror(infile));
		return EXIT_FAILURE;
	}

	if (!(outfile = sf_open(outfile_name, SFM_WRITE, &sfinfo))) {
		fprintf(stderr, "error opening file %s: %s\n", outfile_name, sf_strerror(outfile));
		sf_close(infile);
		return EXIT_FAILURE;
	}

	// ---------- reading and writing ----------
	double input_buffer[CHUNK_SIZE];
	double output_buffer[CHUNK_SIZE];
	
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

	printf("done\n");

	sf_close(infile);
	sf_close(outfile);
}
