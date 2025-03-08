#include <iostream>
#include <bits/stdc++.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iomanip>
#include <SDL2/SDL.h>
using namespace std;

class SC8E{
	public:
		typedef unsigned char BYTE;
		typedef unsigned int WORD;

	private:
		BYTE mem[0xFFF];
		BYTE reg[16];
		WORD addrI;
		WORD pc;
		vector<WORD> stack;
		BYTE display[64][32];
		int windowWidth;
		int windowHeight;
		int pixelScale;
		SDL_Window* window;
		SDL_Renderer* renderer;
	public:
		SC8E(int w, int h, int s){
			SDL_Init(SDL_INIT_VIDEO);
			window = SDL_CreateWindow("SC8E", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			windowWidth = w;
			windowHeight = h;
			pixelScale = s;
		}

		void SC8E_QUIT(){
			SDL_Quit();
			SDL_DestroyWindow(window);
			window = NULL;
			SDL_DestroyRenderer(renderer);
			renderer = NULL;
		}

		void SC8E_ClearScreen(){
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);
		}

		void SC8E_DrawPixel(int x, int y){
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderDrawPoint(renderer, x, y);
		}

		void SC8E_Render(){
			SDL_RenderPresent(renderer);
		}

		int SYS_RESET(const char* path){
			addrI = 0;
			pc = 0x200;
			memset(reg, 0, sizeof(reg));
		
			FILE *f = fopen(path, "rb");
			if(f != NULL){
				fread(&mem[0x200], 0xFFF, 1, f);
				fclose(f);
				return 0;
			}else{
				return 1;
			}
		}

		WORD SYS_GETOP(){
			WORD opcode = mem[pc];
			opcode <<= 8;
			opcode |= mem[pc+1];
			pc += 2;
			return opcode;
		}

		void OP_00E0(){
			SC8E_ClearScreen();
		}
		void OP_00EE(){
			return;
		}
		void OP_1NNN(WORD opcode){
			pc = opcode & 0x0FFF;
		}
		void OP_2NNN(WORD opcode){
			stack.push_back(pc);
			pc = opcode & 0xFFF;
		}
		void OP_3XNN(WORD opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			BYTE NN = opcode & 0x00FF;
			if(reg[X] == NN){
				pc += 2;
			}
		}
		void OP_4XNN(WORD opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			BYTE NN = opcode & 0x00FF;
			if(reg[X] != NN){
				pc += 2;
			}
		}
		void OP_5XY0(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[X] == reg[Y]){
				pc += 2;
			}
		}
		void OP_6XNN(WORD opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			reg[X] = opcode & 0x00FF;
		}
		void OP_7XNN(WORD opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			reg[X] += opcode & 0x00FF;
		}
		void OP_8XY0(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] = Y;
		}
		void OP_8XY1(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] |= Y;
		}
		void OP_8XY2(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] &= Y;
		}
		void OP_8XY3(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] ^= Y;
		}
		void OP_8XY4(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] = X+Y;
		}
		void OP_8XY5(WORD opcode){
			reg[0xF] = 1;
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[Y] > reg[X]){
				reg[0xF] = 0;
			}else{
				reg[X] = X-Y;
			}
		}
		void OP_8XY6(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] >>= 1;
		}
		void OP_8XY7(WORD opcode){
			reg[0xF] = 1;
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[Y] < reg[X]){
				reg[0xF] = 0;
			}else{
				reg[X] = Y-X;
			}
		}
		void OP_8XYE(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] <<= 1;
		}
		void OP_9XY0(WORD opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[X] != reg[Y]){
				pc += 2;
			}
		}

		void SYS_DECODE(WORD opcode){
			switch(opcode & 0xF000){
				case 0x1000:
					OP_1NNN(opcode);
					break;
				case 0x2000:
					OP_2NNN(opcode);
					break;
				case 0x3000:
					OP_3XNN(opcode);
					break;
				case 0x4000:
					OP_4XNN(opcode);
					break;
				case 0x5000:
					OP_5XY0(opcode);
					break;
				case 0x6000:
					OP_6XNN(opcode);
					break;
				case 0x7000:
					OP_7XNN(opcode);
					break;
				case 0x8000:
					switch(opcode & 0x000F){
						case 0x0000:
							OP_8XY0(opcode);
							break;
						case 0x0001:
							OP_8XY1(opcode);
							break;
						case 0x0002:
							OP_8XY2(opcode);
							break;
						case 0x0003:
							OP_8XY3(opcode);
							break;
						case 0x0004:
							OP_8XY4(opcode);
							break;
						case 0x0005:
							OP_8XY5(opcode);
							break;
						case 0x0006:
							OP_8XY6(opcode);
							break;
						case 0x0007:
							OP_8XY7(opcode);
							break;
						case 0x000E:
							OP_8XYE(opcode);
							break;
					}
					break;
				case 0x9000:
					OP_9XY0(opcode);
					break;
				case 0x0000:
					switch(opcode & 0x000F){
						case 0x0000:
							OP_00E0();
							break;
						case 0x000E:
							OP_00EE();
							break;
					}
					break;
			}
		}
};


int main(){
	SC8E em = SC8E(640, 480, 1);

	cout << " ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ " << endl;
	cout << "▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌" << endl;
	cout << "▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ " << endl;
	cout << "▐░▌          ▐░▌          ▐░▌       ▐░▌▐░▌          " << endl;
	cout << "▐░█▄▄▄▄▄▄▄▄▄ ▐░▌          ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ " << endl;
	cout << "▐░░░░░░░░░░░▌▐░▌           ▐░░░░░░░░░▌ ▐░░░░░░░░░░░▌" << endl;
	cout << " ▀▀▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ " << endl;
	cout << "          ▐░▌▐░▌          ▐░▌       ▐░▌▐░▌          " << endl;
	cout << " ▄▄▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ " << endl;
	cout << "▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌" << endl;
	cout << " ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀ " << endl;

	string input;
	cout << "[Rom Path]: ";
	cin >> input;
	char pathArray[input.length() + 1];
	strcpy(pathArray, input.c_str());
	
	if(input == "" || input == "!q"){
		cout << "SC8E_LOG: Program Terminated Successfully" << endl;
		return 0;
	}else if(em.SYS_RESET(pathArray) == 0){
		cout << "SC8E_LOG: Rom Loaded Successfully" << endl;
	}else{
		cerr << "SC8E_ERROR: Rom Could Not Be Loaded Properly" << endl;
		return 0;
	}
	bool run = true;
	SDL_Event e;
	while(run){
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT){
				run = false;
			}
		}
		em.SC8E_ClearScreen();
		em.SYS_DECODE(em.SYS_GETOP());
		em.SC8E_Render();
	}

	em.SC8E_QUIT();
	return 0;
}
