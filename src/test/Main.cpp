#include "TestGame.h"

#include <thread>
#include <chrono>

int count = 0;
Uint32 ticks = 0;

Uint32 doCount(Uint32 interval, void *param) {
	count++;
	//std::cout << count << " " << std::endl;
	return interval;
}

Uint32 doReset(Uint32 interval, void *param) {
	Uint32 delta = SDL_GetTicks() - ticks;
	std::cout << count << " " << delta << std::endl;
	count = 0;
	ticks = SDL_GetTicks();
	return interval;
}

void doCount() {
	count++;
}

void doReset() {
	Uint32 delta = SDL_GetTicks() - ticks;
	std::cout << count << " " << delta << std::endl;
	count = 0;
	ticks = SDL_GetTicks();
}

class Task {
	public:
		void(*func)(void);
		int period;
		double lastRun;
};

bool stop = false;

using namespace std::chrono;

class ScheduledExecutor {
	private:
		std::vector<Task*> tasks;
	public:
		void submit(void(*func)(void), int period) {
			Task * t = new Task();
			t->func = func;
			t->period = period;
			t->lastRun = 0;

			tasks.push_back(t);
		}

		void run() {

			Uint32 time = 0;

			while (!stop) {

				for (auto t : tasks) {
					if (time - t->lastRun >= t->period) {
						t->func();
						t->lastRun = time;
					}
				}

				Uint32 timeout = SDL_GetTicks() + 1;
				while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

				time++;
			}


			/*double time = 0.0;
			while (!stop) {
				
				for (auto t : tasks) {
					if (time - t->lastRun >= t->period) {
						t->func();
						t->lastRun = time;
					}
				}

				SDL_Delay(16);

				time += 20;
			}*/

		}
};

int main(int argc, char * args[]) {

	ScheduledExecutor exec;

	exec.submit(doCount, 20);
	exec.submit(doReset, 1000);

	//SDL_AddTimer(1000/60, doCount, 0);
	//SDL_AddTimer(1000, doReset, 0);


	std::thread bgThread(&ScheduledExecutor::run, exec);

	

	/*try {
		TestGame game;
		game.runMainLoop();
	}
	catch (EngineException & e) {
		std::cout << e.what() << std::endl;
		getchar();
	}*/

	getchar();
	stop = true;

	bgThread.join();

	getchar();
	return 0;
}