/* Medion's snake game */
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <ctime>
#include <vector>

class Snake
{
public:
	std::vector<std::vector<int>> sParts;
	char body = 'o', head = 'O', dead_body = '-', dead_head = 'X';
	Snake();
	void Init(int x, int y);
};

class Game 
{
public:
	enum direct {UP = 1, DOWN, LEFT, RIGHT};
	bool isRunning = true;
	Game(int width = 40, int height = 20);
	void Init(Snake& aSnake);
	void GetInput();
	void Update(Snake& aSnake);
	void Render(const Snake& aSnake) const;
	void EndOrStart();
	void SetTimer();
	void TimeDelay();
private:
	const unsigned short M_WIDTH, M_HEIGHT;
	const char M_FRUIT = '*', M_BORDERS = '#', M_BACKGROUND = ' ';
	unsigned int m_score, m_tale, m_timeStart, m_fps, m_frameDelay;
	int m_fruitSpot[2];
	short m_xOldSpeed, m_yOldSpeed;
	direct m_direct;
	void SpotFruit();
};

bool yesOrNo(const char* string);

int main()
{
	unsigned short WIDTH = 40, HEIGHT = 20;
	Game aGame (WIDTH, HEIGHT);
	Snake aSnake;
	while (true) {
		aGame.Init(aSnake);
		while (aGame.isRunning) {
			aGame.SetTimer();
			/* User's input */
			aGame.GetInput();
			/* Logic */
			aGame.Update(aSnake);
			/* Rendering */
			aGame.Render(aSnake);
			/* Frame manipulation */
			aGame.TimeDelay();
		}
		if (yesOrNo("Do u wanna restart the game? (y/n)"))
			aGame.EndOrStart();
		else
			break;	
	}
	return 0;
}

Game::Game(int width, int height):M_WIDTH(width), M_HEIGHT(height) 
{
	srand(static_cast<int>(time(0))); // робим посів
}

void Game::Init(Snake& aSnake)
{
	aSnake.Init(M_WIDTH*0.5, M_HEIGHT*0.5);
	SpotFruit();
	m_direct = RIGHT;
	m_score = 0;
	m_tale = 1;
	m_fps = 15;
}

void Game::GetInput()
{
	if (_kbhit()) {
		switch (tolower(_getch())) {
			case 'w':
				m_direct = UP;
				break;
			case 's':
				m_direct = DOWN;
				break;
			case 'a':
				m_direct = LEFT;
				break;
			case 'd':
				m_direct = RIGHT;
				break;
		}
	}
}

void Game::Update(Snake& aSnake)
{
	if (aSnake.sParts[0][0] == 0 || aSnake.sParts[0][1] == 0 || 
		aSnake.sParts[0][0] == M_WIDTH-1 || aSnake.sParts[0][1] == M_HEIGHT-1) {
		EndOrStart();
	} else if (aSnake.sParts[0][0] == m_fruitSpot[0] && aSnake.sParts[0][1] == m_fruitSpot[1]) {
		m_tale++;
		m_score += 100;
		if (m_fps < 60)
			m_fps+=2;
		SpotFruit();
	}

	while (m_tale > aSnake.sParts.size())
		aSnake.sParts.push_back({0, 0});

	std::vector<int> prevPoints = aSnake.sParts[0], savePoints;
	short xSpeed = 0, ySpeed = 0;
	switch (m_direct) {
		case UP:
			ySpeed--;
			break;
		case DOWN:
			ySpeed++;
			break;
		case LEFT:
			xSpeed--;
			break;
		case RIGHT:
			xSpeed++;
			break;
	}

	/* Милиці */
	if (xSpeed != 0) {
		if (m_tale > 1 && (aSnake.sParts[0][0] + xSpeed) == aSnake.sParts[1][0]) {
			aSnake.sParts[0][0] += m_xOldSpeed;
		} else {
			aSnake.sParts[0][0] += xSpeed;
			m_xOldSpeed = xSpeed;
		}
	} else {
		if (m_tale > 1 && (aSnake.sParts[0][1] + ySpeed) == aSnake.sParts[1][1]) {
			aSnake.sParts[0][1] += m_yOldSpeed;
		} else {
			aSnake.sParts[0][1] += ySpeed;
			m_yOldSpeed = ySpeed;
		}
	}
	/* Милиці end. */

	for (unsigned int i = 2; i < m_tale; i++) {
		if (aSnake.sParts[0] == aSnake.sParts[i])
			EndOrStart();
	}

	for (unsigned int i = 1; i < m_tale; i++) {
		savePoints = aSnake.sParts[i];
		aSnake.sParts[i] = prevPoints;
		prevPoints = savePoints;
	}
}

void Game::SpotFruit()
{
	m_fruitSpot[0] = rand() % (M_WIDTH - 2) + 1;
	m_fruitSpot[1] = rand() % (M_HEIGHT - 2) + 1;
}

void Game::Render(const Snake& aSnake) const
{
	std::system("CLS");
	bool founded;
	std::cout << "Score: " << m_score << std::endl;

	for (unsigned int y = 0; y < M_HEIGHT; y++) {
		for (unsigned int x = 0; x < M_WIDTH; x++) {
			founded = false;
			if ((y*x) == 0 || y == M_HEIGHT - 1 || x == M_WIDTH - 1) {
				std::cout << M_BORDERS;
			} else if (x == m_fruitSpot[0] && y == m_fruitSpot[1]) {
				std::cout << M_FRUIT;
			} else {
				char body, head;
				if (!isRunning) {
					head = aSnake.dead_head;
					body = aSnake.dead_body;
				} else {
					head = aSnake.head;
					body = aSnake.body;
				}
				for (unsigned int j = 0; j < aSnake.sParts.size(); j++) {
					if (x == aSnake.sParts[j][0] && y == aSnake.sParts[j][1]) {
						founded = true;
						if (j == 0)
							std::cout << head;
						else
							std::cout << body;
					}
				}
				if (!founded)
					std::cout << M_BACKGROUND;
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Game::EndOrStart()
{
	isRunning = !isRunning;
}

void Game::SetTimer()
{
	m_timeStart = std::time(0);
}

void Game::TimeDelay()
{
	m_frameDelay = 1e3 / m_fps;
	int timeChange = std::time(0) - m_timeStart;
	if (m_frameDelay > timeChange)
		Sleep(m_frameDelay - timeChange);
}

// Snake 
Snake::Snake()
{
	sParts.reserve(30);
}

void Snake::Init(int x, int y)
{
	if (!sParts.empty())
		sParts.clear();

	sParts.push_back({x, y});
}

bool yesOrNo(const char* string = "")
{
	char resp;
	do {
		std::cout << string << std::endl;
		std::cin >> resp;

		if (resp == 'y' || resp == 'Y')
			return true;
		
		if (resp == 'n' || resp == 'N')
			return false;

	} while(true);
}