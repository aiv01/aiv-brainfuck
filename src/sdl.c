#include "brainfuck.h"

#include <SDL2/SDL.h>

static struct brainfuck_sdl_vm {
	struct brainfuck_vm vm;
	unsigned int window_width;
	unsigned int window_height;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture *texture;
} brainfuck_sdl_vm;

#define IVT_REDRAW	0
#define IVT_SLEEP	1
#define IVT_SLEEP_VALUE	2

static int pre_hook(struct brainfuck_vm *vm)
{
	struct brainfuck_sdl_vm *sdl_vm = (struct brainfuck_sdl_vm *)vm;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				return -1;
			}
		}
	}
	return 0;
}

static int post_hook(struct brainfuck_vm *vm)
{
	struct brainfuck_sdl_vm *sdl_vm = (struct brainfuck_sdl_vm *)vm;
	unsigned char *bitmap = (unsigned char *) (vm->memory + vm->memory_size - (sdl_vm->window_width * 4 * sdl_vm->window_height));
	char *ivt = (char *)bitmap - 8;

	if (ivt[IVT_REDRAW] == 1) {
		void *pixels = NULL;
		int pitch = 0;
		if (SDL_LockTexture(sdl_vm->texture, NULL, (void **) &pixels, &pitch) < 0) {
			fprintf(stderr, "unable to lock SDL Texture\n");
			return -1;
		}
		memcpy(pixels, bitmap, (sdl_vm->window_width * 4 * sdl_vm->window_height));
                SDL_UnlockTexture(sdl_vm->texture);

		for(int i =0;i<28;i++) {
			printf("[%u] ", bitmap[i]);
		}
		printf("\n");

                SDL_RenderClear(sdl_vm->renderer);
                SDL_RenderCopy(sdl_vm->renderer, sdl_vm->texture, NULL, NULL);
                SDL_RenderPresent(sdl_vm->renderer);
		// turn off interrupt
		ivt[IVT_REDRAW] = 0;
	}

	if (ivt[IVT_SLEEP] == 1) {
		SDL_Delay((unsigned char)ivt[IVT_SLEEP_VALUE]);
		// turn off interrupt
		ivt[IVT_SLEEP] = 0;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int ret = -1;

	if (argc < 4) {
		fprintf(stderr, "plase specify the brainfuck vm memory size and window dimensions\n");
		return -1;
	}

	struct brainfuck_sdl_vm sdl_vm;
	sdl_vm.window_width = atoi(argv[2]);
	sdl_vm.window_height = atoi(argv[3]);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "unable to initialize SDL\n");
		return -1;
	}
		

	sdl_vm.window = SDL_CreateWindow("AIV Brainfuck SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sdl_vm.window_width, sdl_vm.window_height, 0);
	if (!sdl_vm.window) {
		fprintf(stderr, "unable to initialize SDL Window\n");
		goto end_sdl;
	}

	sdl_vm.renderer = SDL_CreateRenderer(sdl_vm.window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	if (!sdl_vm.renderer) {
		fprintf(stderr, "unable to initialize SDL Renderer\n");
		goto end_sdl;
	}

	sdl_vm.texture = SDL_CreateTexture(sdl_vm.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, sdl_vm.window_width, sdl_vm.window_height);
	if (!sdl_vm.renderer) {
		fprintf(stderr, "unable to initialize SDL Texture\n");
		goto end_sdl_renderer;
	}
	

	if (brainfuck_init(&sdl_vm.vm, atoi(argv[1]) + 8 + (sdl_vm.window_width * 4 * sdl_vm.window_height))) {
		fprintf(stderr, "unable to initialize brainfuck vm memory\n");
		goto end_bf;
	}

	sdl_vm.vm.pre_hook = pre_hook;
	sdl_vm.vm.post_hook = post_hook;

	fprintf(stdout, "AIV brainfuck SDL VM initialized with %u bytes of memory (window size %ux%u)\n", sdl_vm.vm.memory_size, sdl_vm.window_width, sdl_vm.window_height);

	// manually call the pre_hook to complete the window setup
	pre_hook(&sdl_vm.vm);


	if (argc > 4) {
		FILE *source = fopen(argv[4], "r");
		if (!source) {
			fprintf(stderr, "unable to open file\n");
			goto end_bf;
		}

		fseek(source, 0, SEEK_END);
		long rlen = ftell(source);
		fseek(source, 0, SEEK_SET);

		char *code = (char *)malloc(rlen);
		if (!code) {
			fprintf(stderr, "unable to allocate code memory\n");
			goto end_bf;
		}

		fread(code, 1, rlen, source);

		unsigned int opcode_err_pos = 0;

		ret = brainfuck_run(&sdl_vm.vm, code, rlen, &opcode_err_pos);
                if (ret < 0) {
                	fprintf(stderr, "error while running opcode '%c' at position %u\n", code[opcode_err_pos], opcode_err_pos);
                }
		// free resources
                free(code);
		fclose(source);
	} else {
		unsigned int line_number = 0;
		for (;;) {
			char line_buf[4096];
			char *line = fgets(line_buf, 4096, stdin);
			if (!line)
				break;
			unsigned int opcode_err_pos = 0;
			ret = brainfuck_run(&sdl_vm.vm, line, strlen(line), &opcode_err_pos);
			if (ret < 0) {
				fprintf(stderr, "error while running opcode '%c' at position %u line: %u \n", line[opcode_err_pos], opcode_err_pos, line_number);
				break;
			}
			line_number++;
		}
	}

end_bf:
	brainfuck_destroy(&sdl_vm.vm);
end_sdl_renderer:
	SDL_DestroyRenderer(sdl_vm.renderer);
end_sdl:
	SDL_Quit();

	return ret;
}
