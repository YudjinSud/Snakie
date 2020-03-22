#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <windows.h>

#define width 640
#define height 480
#define cellSize 10
#define step 10

struct Cell {
	sf::RectangleShape rect;
	int x;
	int y;
	int direction;
};


struct Snake {
	std::vector<Cell*> body;
	int len = 0; // current number of cells
	int tail = 0;
};



void printDebugSnake(Snake *snake) {
	system("cls");
	for (int i = 0; i < snake->len; i++) {
		std::cout << "Cell " << i << "\n";
		std::cout << "X :: " << snake->body[i]->x << " Y :: " << snake->body[i]->y << "\n";
		std::cout << "Direction :: " << snake->body[i]->direction << "\n";
		std::cout << "=========================\n";
	}
}


void drawCell(sf::RenderWindow &window, Cell *cell) {
	//why pointer - cell* ?
	//for now i suppose there will be a lot of cells - certainly.
	//And every game loop we have to pass all that data to draw() and update() stuff
	cell->rect.setFillColor(sf::Color::Red);
	cell->rect.setOutlineColor(sf::Color::Black);
	cell->rect.setOutlineThickness(1);
	cell->rect.setPosition(cell->x, cell->y);
	cell->rect.setSize(sf::Vector2f(cellSize, cellSize));
	window.draw(cell->rect);
}


void drawSnake(sf::RenderWindow &window, Snake *snake) {
	for (int i = 0; i < snake->body.size(); i++) {
		drawCell(window, snake->body[i]);
	}
	window.display();
}


void addCell(Snake *snake) {
	Cell *push = new Cell;
	Cell *tail = snake->body[snake->len-1];
	push->x = tail->x - cellSize;
	push->y = tail->y;
	push->direction = 0;
	snake->len++;
	snake->tail++;
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
	if (snake->tail <= 0) {
		snake->tail = snake->len - 1;
	}
	int len = snake->body.size();
	snake->body[snake->tail]->x = snake->body[0]->x;
	snake->body[snake->tail]->y = snake->body[0]->y;
	snake->body[snake->tail]->rect = snake->body[0]->rect;
	snake->body[snake->tail]->direction = snake->body[0]->direction;
	snake->tail -= 1;
}



void updateSnakeCells(Snake *snake) {
	copyLastToFirst(snake);
	switch (snake->body[0]->direction) {
		case 1:
			snake->body[0]->y -= step;
			break;
		case 2:
			snake->body[0]->y += step;
			break;
		case 3:
			snake->body[0]->x -= step;
			break;
		case 4:
			snake->body[0]->x += step;
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


int collisionDetection(Cell *head) {
	if (head->x > width - cellSize)
	{
		return 2;
	}
	else if (head->x < cellSize)
	{
		return 1;
	}
	else if (head->y > height - cellSize)
	{
		return 4;
	}
	else if (head->y < cellSize)
	{
		return 3;
	}
	return 0;
}


void rotateHead(Snake *snake, int direction) {
	//int colDet = collisionDetection(snake->body[0]);
	//if (colDet) {
	//	direction = colDet;
	//}
	//need to check if direction is possible
	int relation12 = checkCellsRelation(snake->body[0], snake->body[1]);
	int relation23 = checkCellsRelation(snake->body[1], snake->body[2]); // check if right or left rotation is even possible
	// because maybe there some 3'd cell;
	if (snake->body[0]->direction != direction && direction == 1 && relation12 != 1) //up
	{
		snake->body[0]->x = snake->body[snake->tail+1]->x; // after some iterations 2'nd cell is near tail...
		//need to know which cell now is closest to head.
		snake->body[0]->y -= cellSize;
		snake->body[0]->direction = direction;
	}
	else if(snake->body[0]->direction != direction && direction == 2 && relation12 != 2) //down
	{
		snake->body[0]->x = snake->body[snake->tail+1]->x;
		snake->body[0]->y += cellSize;
		snake->body[0]->direction = direction;
	}
	else if (snake->body[0]->direction != direction && direction == 3 && relation12 != 3 && relation23 != 3) // left
	{
		snake->body[0]->x -= cellSize;
		snake->body[0]->y = snake->body[snake->tail+1]->y;
		snake->body[0]->direction = direction;
	}
	else if (snake->body[0]->direction != direction && direction == 4 && relation12 != 4 && relation23 != 4) // right
	{
		snake->body[0]->x += cellSize;
		snake->body[0]->y = snake->body[snake->tail+1]->y;
		snake->body[0]->direction = direction;
	}
	printDebugSnake(snake);
}


//@utility
void fillSnake(Snake *snake, int n) {
	for (int i = 0; i < n; i++) {
		addCell(snake);
	}
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(width, height), "Snakie!");
	sf::Event event;

	Cell *cell1 = new Cell;
	cell1->x = 100;
	cell1->y = 100;
	cell1->direction = 4;
	

	Snake *snake = new Snake; //again, we don't know how much of stack available here. Using heap therefore.
	snake->body = std::vector<Cell*>();
	snake->body.push_back(cell1);
	snake->len = 1;
	std::cout << snake->len << "\n";
	fillSnake(snake, 30);

	std::cout << snake->len << "\n";


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
		//std::cout << direction << '\n';
		updateSnakeCells(snake);
		//calculateCellsDirection(snake, direction);
		drawSnake(window, snake);
		window.clear();
		sleep(sf::milliseconds(30));
	}

	return 0;
}