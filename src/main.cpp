#include <iostream>
#include <bits/stdc++.h>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
using namespace std;

class SC8E{
	private:
		unsigned char fontset[80] = {
			0xF0, 0x90, 0x90, 0x90, 0xF0,
			0x20, 0x60, 0x20, 0x20, 0x70,
			0xF0, 0x10, 0xF0, 0x80, 0xF0,
			0xF0, 0x10, 0xF0, 0x10, 0xF0,
			0x90, 0x90, 0xF0, 0x10, 0x10,
			0xF0, 0x80, 0xF0, 0x10, 0xF0,
			0xF0, 0x80, 0xF0, 0x90, 0xF0,
			0xF0, 0x10, 0x20, 0x40, 0x40,
			0xF0, 0x90, 0xF0, 0x90, 0xF0,
			0xF0, 0x90, 0xF0, 0x10, 0xF0,
			0xF0, 0x90, 0xF0, 0x90, 0x90,
			0xE0, 0x90, 0xE0, 0x90, 0xE0,
			0xF0, 0x80, 0x80, 0x80, 0xF0,
			0xE0, 0x90, 0x90, 0x90, 0xE0,
			0xF0, 0x80, 0xF0, 0x80, 0xF0,
			0xF0, 0x80, 0xF0, 0x80, 0x80
		};
		unsigned short opcode;
		unsigned char mem[0xFFF];
		unsigned char reg[0xF];
		unsigned char delayTimer;
		unsigned char soundTimer;
		unsigned short addrI;
		unsigned short pc;
		unsigned short stack[0xF];
		unsigned short sp;
	        unsigned char display[64][32];
		unsigned char keypad[0xF];
		unsigned int windowWidth;
		unsigned int windowHeight;
		unsigned int key;
		float pixelScale;
		SDL_Window* window;
		SDL_Renderer* renderer;
	public:
		SC8E(int w, int h, float s){
			SDL_Init(SDL_INIT_VIDEO);
			window = SDL_CreateWindow("SC8E", w, h, 0);
			renderer = SDL_CreateRenderer(window, NULL);
			windowWidth = w;
			windowHeight = h;
			pixelScale = s;	
		}
		void setTitle(const char* title){
			SDL_SetWindowTitle(window, title);
		}
		void SC8E_QUIT(){
			SDL_DestroyRenderer(renderer);
			renderer = NULL;
			SDL_DestroyWindow(window);
			window = NULL;
			SDL_Quit();
		}
		void SC8E_ClearScreen(){
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);
		}
		void SC8E_Draw(int x, int y){
			if(display[x][y] == 1){
				SDL_FRect pixel = {x*pixelScale, y*pixelScale, pixelScale, pixelScale};
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderFillRect(renderer, &pixel);
			}
		}
		void SC8E_Render(){
			SDL_RenderPresent(renderer);
		}
		
		int SYS_INIT(const char* path){
			addrI = 0;
			pc = 0x200;
			memset(reg, 0, sizeof(reg));
			memset(mem, 0, sizeof(mem));
			memset(display, 0, sizeof(display));
			memset(stack, 0, sizeof(stack));
			for(int i = 0; i < 80; i++){
				mem[i] = fontset[i];
			}	
			FILE *f = fopen(path, "rb");
			if(f != NULL){
				fread(&mem[0x200], 0xFFF, 1, f);
				fclose(f);
				return 0;
			}else{
				return 1;
			}
			delayTimer = 0;
			soundTimer = 0;
		}
		void SYS_CLEAR(){
			for(int x = 0; x < 64; x++){
				for(int y = 0; y < 32; y++){
					display[x][y] = 0;
				}

			}
		}
		unsigned short SYS_GETOP(){
			opcode = mem[pc];
			opcode <<= 8;
			opcode |= mem[pc+1];
			pc += 2;
			return opcode;
		}
		void OP_00E0(){
			SYS_CLEAR();
		}
		void OP_00EE(){
			return;
		}
		void OP_1NNN(unsigned short opcode){
			pc = opcode & 0x0FFF;
		}
		void OP_2NNN(unsigned short opcode){
			stack[sp] = pc;
			sp++;
			pc = opcode & 0xFFF;
		}
		void OP_3XNN(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			unsigned short NN = opcode & 0x00FF;
			if(reg[X] == NN){
				pc += 2;
			}
		}
		void OP_4XNN(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			unsigned short NN = opcode & 0x00FF;
			if(reg[X] != NN){
				pc += 2;
			}
		}
		void OP_5XY0(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[X] == reg[Y]){
				pc += 2;
			}
		}
		void OP_6XNN(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			reg[X] = opcode & 0x00FF;
		}
		void OP_7XNN(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			reg[X] += opcode & 0x00FF;
		}
		void OP_8XY0(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] = reg[Y];
		}
		void OP_8XY1(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] |= reg[Y];
		}
		void OP_8XY2(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] &= reg[Y];
		}
		void OP_8XY3(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] ^= reg[Y];
		}
		void OP_8XY4(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] = reg[X] + reg[Y];
		}
		void OP_8XY5(unsigned short opcode){
			reg[0xF] = 1;
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[Y] > reg[X]){
				reg[0xF] = 0;
			}else{
				reg[X] = reg[X] - reg[Y];
			}
		}
		void OP_8XY6(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] >>= 1;
		}
		void OP_8XY7(unsigned short opcode){
			reg[0xF] = 1;
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[Y] < reg[X]){
				reg[0xF] = 0;
			}else{
				reg[X] = reg[Y] - reg[X];
			}
		}
		void OP_8XYE(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			reg[X] <<= 1;
		}
		void OP_9XY0(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			if(reg[X] != reg[Y]){
				pc += 2;
			}
		}
		void OP_ANNN(unsigned short opcode){
			addrI = opcode & 0x0FFF;
		}
		void OP_BNNN(unsigned short opcode){
			pc = reg[0] + (opcode & 0x0FFF);
		}
		void OP_CXNN(unsigned short opcode){
			int X = opcode & 0x0F00;
			reg[X] = (rand() % 256) & (opcode & 0x00FF);
		}
		void OP_DXYN(unsigned short opcode){
			int X = opcode & 0x0F00;
			int Y = opcode & 0x00F0;
			X >>= 8;
			Y >>= 4;
			int height = opcode & 0x000F;
			int originX = reg[X];
			int originY = reg[Y];
			reg[0xF] = 0;
			for(int y = 0; y < height; y++){
				unsigned char spriteData = mem[addrI+y];
				int xBit = 7;
				for(int x = 0; x < 8; x++, xBit--){
					int mask = 1 << xBit;
					if(spriteData & mask){
						int posX = originX + x;
						int posY = originY + y;
						if(display[posX][posY] == 1){
							reg[0xF] = 1;
						}
						display[posX][posY] ^= 1;
					}	
				}
			}
		}
		void OP_EX9E(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			if(key == reg[X]){
				pc += 4;
			}else{
				pc += 2;
			}
		}
		void OP_EXA1(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			if(key != reg[X]){
				pc += 4;
			}else{
				pc += 2;
			}
		}
		void OP_FX0A(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			if(key != 0x00){
				reg[X] = key;
				pc += 2;
			}
		}
		void OP_FX1E(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			addrI += reg[X];
		}
		void OP_FX07(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			reg[X] = delayTimer;
		}
		void OP_FX15(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			delayTimer = reg[X];
		}
		void OP_FX18(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			soundTimer = reg[X];
		}
		void OP_FX29(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			addrI = reg[X] * 5;
		}
		void OP_FX33(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			int regValue = reg[X];
			mem[addrI] = regValue % 100;
			mem[addrI + 1] = (regValue / 10) % 10;
			mem[addrI + 2] = regValue % 10;
		}
		void OP_FX55(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			for(int i = 0; i <= X; i++){
				mem[addrI+i] = reg[i];
			}
			addrI = addrI+X+1;
		}
		void OP_FX65(unsigned short opcode){
			int X = opcode & 0x0F00;
			X >>= 8;
			for(int i = 0; i <= X; i++){
				reg[i] = mem[addrI+i];
			}
			addrI = addrI+X+1;	
		}
		void SYS_DECODE(unsigned short opcode){
			switch(opcode & 0xF000){
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
				case 0xA000:
					OP_ANNN(opcode);
					break;
				case 0xB000:
					OP_BNNN(opcode);
					break;
				case 0xC000:
					OP_CXNN(opcode);
					break;
				case 0xD000:
					OP_DXYN(opcode);
					break;
				case 0xE000:
					switch(opcode & 0x00FF){
						case 0x009E:
							OP_EX9E(opcode);
							break;
						case 0x00A1:
							OP_EXA1(opcode);
							break;
					}
				case 0xF000:
					switch(opcode & 0x00FF){
						case 0x0007:
							OP_FX07(opcode);
							break;
						case 0x0015:
							OP_FX15(opcode);
							break;
						case 0x0018:
							OP_FX18(opcode);
							break;
						case 0x0029:
							OP_FX29(opcode);
							break;
						case 0x001E:
							OP_FX1E(opcode);
							break;
						case 0x0033:
							OP_FX33(opcode);
							break;
						case 0x0055:
							OP_FX55(opcode);
							break;
						case 0x0065:
							OP_FX65(opcode);
							break;
						case 0x000A:
							OP_FX0A(opcode);
					}
					break;
				default:
					cout << "SC8E_ERROR: ILLEGAL INSTRUCTION!" << endl;
			}
			if(delayTimer > 0){
				delayTimer--;
			}
			if(soundTimer > 0){
				if(soundTimer==1){
					printf("\a");
				}
				soundTimer--;
			}
		}
		void SC8E_HandleKeyEvents(SDL_Event event){
			switch(event.key.key){
				case SDLK_1:
					key = 0x01;
					break;
				case SDLK_2:
					key = 0x02;
					break;
				case SDLK_3:
					key = 0x03;
					break;
				case SDLK_4:
					key = 0x04;
					break;
				case SDLK_5:
					key = 0x05;
					break;
				case SDLK_6:
					key = 0x06;
					break;
				case SDLK_7:
					key = 0x07;
					break;
				case SDLK_8:
					key = 0x08;
					break;
				case SDLK_9:
					key = 0x09;
					break;
				case SDLK_A:
					key = 0x0A;
					break;
				case SDLK_B:
					key = 0x0B;
					break;
				case SDLK_C:
					key = 0x0C;
					break;
				case SDLK_D:
					key = 0x0D;
					break;
				case SDLK_E:
					key = 0x0E;
					break;
				case SDLK_F:
					key = 0x0F;
					break;
				default:
					key = 0x00;	
			}
		}
		void SC8E_ResetKeyEvent(){
			key = 0x00;
		}
};


int main(){
	SC8E em = SC8E(764, 384, 12);

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

	if(input == "" || input == "q"){
		cout << "SC8E_LOG: Program Terminated Successfully" << endl;
		return 0;
	}else if(em.SYS_INIT(pathArray) == 0){
		cout << "SC8E_LOG: Rom Loaded Successfully" << endl;
		em.setTitle(input.c_str());
	}else{
		cerr << "SC8E_ERROR: Rom Could Not Be Loaded Properly" << endl;
		return 0;
	}
	bool run = true;
	SDL_Event e;
	while(run){
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_EVENT_QUIT){
				run = false;
			}else if(e.type == SDL_EVENT_KEY_DOWN){
				em.SC8E_HandleKeyEvents(e);
			}else if(e.type == SDL_EVENT_KEY_UP){
				em.SC8E_ResetKeyEvent();
			}
		}
		em.SC8E_ClearScreen();
		em.SYS_DECODE(em.SYS_GETOP());
		for(int x = 0; x < 64; x++){
			for(int y = 0; y < 32; y++){
				em.SC8E_Draw(x, y);
			}
		}
		em.SC8E_Render();
		SDL_Delay((1/60)*1000);
	}
	em.SC8E_QUIT();
	return 0;
}
