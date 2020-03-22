#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <deque>
#include <windows.h>

#define fieldSize 512
#define cellSize 15
#define step 15

struct Cell {
	sf::RectangleShape rect;
	int x;
	int y;
	int direction;
};


struct Snake {
	std::deque<Cell*> body;
};



void printDebugSnake(Snake *snake) {
	//std::cout << "Tail :: " << snake->tail << "\n";
}


void drawCell(sf::RenderWindow &window, Cell *cell, int n) {
	//why pointer - cell* ?
	//for now i suppose there will be a lot of cells - certainly.
	//And every game loop we have to pass all that data to draw() and update() stuff
	cell->rect.setFillColor(sf::Color::Magenta);
	cell->rect.setOutlineColor(sf::Color::Black);
	cell->rect.setOutlineThickness(1);
	cell->rect.setPosition(cell->x, cell->y);
	cell->rect.setSize(sf::Vector2f(cellSize, cellSize));
	window.draw(cell->rect);
}


void drawSnake(sf::RenderWindow &window, Snake *snake) {
	for (int i = 0; i < snake->body.size(); i++) {
		drawCell(window, snake->body[i], i);
	}
	window.display();
}


void addCell(Snake *snake) {
	Cell *push = new Cell;
	Cell *tail = snake->body[snake->body.size() - 1];
	push->x = tail->x - cellSize;
	push->y = tail->y;
	push->direction = 0;
	snake->body.push_back(push);
}


int processKeyboard(sf::Event event, sf::RenderWindow &window) {
	//returns direction of snake
	int direction = 0;
	switch (event.key.code) {
	case sf::Keyboard::Up:
		std::cout << "Up button pressed\n";
		direction = 1;
		break;
	case sf::Keyboard::Down:
		std::cout << "Down button pressed\n";
		direction = 2;
		break;
	case sf::Keyboard::Left:
		std::cout << "Left button pressed\n";
		direction = 3;
		break;
	case sf::Keyboard::Right:
		std::cout << "Right button pressed\n";
		direction = 4;
		break;
	default:
		window.close();
	}
	return direction;
}

void copyLastToFirst(Snake *snake) {
	//only we have to do to animate snake - delete tail and insert it right on the head of sneak
	int size = snake->body.size();
	Cell *push = new Cell;
	push->direction = snake->body[0]->direction;
	push->rect = snake->body[0]->rect;
	push->x  = snake->body[0]->x;
	push->y = snake->body[0]->y;
	snake->body.push_front(push);
	snake->body.pop_back();
}


int checkBoundary(Cell *head) {
	//check if snake is out of game field
	if (head->x > fieldSize - cellSize)
	{
		return -fieldSize;
	}
	else if (head->x < -cellSize)
	{
		return -fieldSize;
	}
	else if (head->y > fieldSize - cellSize)
	{
		return -fieldSize;
	}
	else if (head->y < -cellSize)
	{
		return -fieldSize;
	}
	return 0;
}



void updateSnakeCells(Snake *snake) {
	int delta = checkBoundary(snake->body.front());
	if (delta == 0) {
		delta = step;
	}
	copyLastToFirst(snake);
	switch (snake->body[0]->direction) {
		case 1:
			snake->body[0]->y -= delta;
			break;
		case 2:
			snake->body[0]->y += delta;
			break;
		case 3:
			snake->body[0]->x -= delta;
			break;
		case 4:
			snake->body[0]->x += delta;
			break;
	}
}


int checkCellsRelation(Cell *c1, Cell *c2) {
	//given head of snake and 2'nd cell of snake
	//return the direction of 2'nd according to position of first

	if (c1->x == c2->x && c1->y - cellSize == c2->y) {
		return 1; //above
	}
	else if (c1->x == c2->x && c1->y + cellSize == c2->y) {
		return 2; // below
	}
	else if (c1->x - cellSize == c2->x && c1->y == c2->y) {
		return 3; // leftward
	}
	else if(c1->x + cellSize == c2->x && c1->y == c2->y){
		return 4; //right
	}
}


void rotateHead(Snake *snake, int direction) {
	//need to check if direction is possible
	int relation12 = checkCellsRelation(snake->body[0], snake->body[1]);
	int relation23 = checkCellsRelation(snake->body[1], snake->body[2]); // check if right or left rotation is even possible
	// because maybe there some 3'd cell;
	if (snake->body[0]->direction != direction && direction == 1 && relation12 != 1) //up
	{
		snake->body[0]->direction = direction;
	}
	else if(snake->body[0]->direction != direction && direction == 2 && relation12 != 2) //down
	{
		snake->body[0]->direction = direction;
	}
	else if (snake->body[0]->direction != direction && direction == 3 && relation12 != 3 && relation23 != 3) // left
	{
		snake->body[0]->direction = direction;
	}
	else if (snake->body[0]->direction != direction && direction == 4 && relation12 != 4 && relation23 != 4) // right
	{
		snake->body[0]->direction = direction;
	}
}


//@utility
void fillSnake(Snake *snake, int n) {
	for (int i = 0; i < n; i++) {
		addCell(snake);
	}
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(fieldSize, fieldSize), "Snakie!");
	sf::Event event;

	Cell *cell1 = new Cell;
	cell1->x = 100;
	cell1->y = 100;
	cell1->direction = 4;

	Snake *snake = new Snake; //again, we don't know how much of stack available here. Using heap therefore.
	snake->body = std::deque<Cell*>();
	snake->body.push_back(cell1); 

	fillSnake(snake, 3);

	drawSnake(window, snake);

	int direction = 0;

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				direction = processKeyboard(event, window);
				rotateHead(snake, direction);
			}
		}
		updateSnakeCells(snake);
		drawSnake(window, snake);
		window.clear();
		sleep(sf::milliseconds(50));
	}

	return 0;
}