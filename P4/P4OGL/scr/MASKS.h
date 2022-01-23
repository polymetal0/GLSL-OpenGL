
float identity[] = {
float(0.0), float(0.0), float(0.0),
float(0.0), float(1.0), float(0.0),
float(0.0), float(0.0), float(0.0) };

const float maskFactor = float(1.0 / 16.0);
float blur[] = {
float(1.0 * maskFactor), float(2.0 * maskFactor), float(1.0 * maskFactor),
float(2.0 * maskFactor), float(4.0 * maskFactor), float(2.0 * maskFactor),
float(1.0 * maskFactor), float(2.0 * maskFactor), float(1.0 * maskFactor) };

float sharpen[] = {
float(0.0), float(-1.0), float(0.0),
float(-1.0), float(5.0), float(-1.0),
float(0.0), float(-1.0), float(0.0) };

float emboss[] = {
float(-2.0), float(-1.0), float(0.0),
float(-1.0), float(1.0), float(1.0),
float(0.0), float(1.0), float(2.0) };

float outline[] = {
float(-1.0), float(-1.0), float(-1.0),
float(-1.0), float(8.0), float(-1.0),
float(-1.0), float(-1.0), float(-1.0) };



/*//#define MASK_SIZE 25u
const glm::vec2 texIdx2 = (
	glm::vec2(-2.0,2.0), glm::vec2(-1.0,2.0), glm::vec2(0.0,2.0), glm::vec2(1.0,2.0), glm::vec2(2.0,2.0),
	glm::vec2(-2.0,1.0), glm::vec2(-1.0,1.0), glm::vec2(0.0,1.0), glm::vec2(1.0,1.0), glm::vec2(2.0,1.0),
	glm::vec2(-2.0,0.0), glm::vec2(-1.0,0.0), glm::vec2(0.0,0.0), glm::vec2(1.0,0.0), glm::vec2(2.0,0.0),
	glm::vec2(-2.0,-1.0), glm::vec2(-1.0,-1.0), glm::vec2(0.0,-1.0), glm::vec2(1.0,-1.0), glm::vec2(2.0,-1.0),
	glm::vec2(-2.0,-2.0), glm::vec2(-1.0,-2.0), glm::vec2(0.0,-2.0), glm::vec2(1.0,-2.0), glm::vec2(2.0,-2.0) );
const float maskFactor2 = float (1.0/65.0);
float mask2[] = {
	1.0 * maskFactor2, 2.0 * maskFactor2, 3.0 * maskFactor2,2.0 * maskFactor2, 1.0 * maskFactor2,
	2.0 * maskFactor2, 3.0 * maskFactor2, 4.0 * maskFactor2,3.0 * maskFactor2, 2.0 * maskFactor2,
	3.0 * maskFactor2, 4.0 * maskFactor2, 5.0 * maskFactor2,4.0 * maskFactor2, 3.0 * maskFactor2,
	2.0 * maskFactor2, 3.0 * maskFactor2, 4.0 * maskFactor2,3.0 * maskFactor2, 2.0 * maskFactor2,
	1.0 * maskFactor2, 2.0 * maskFactor2, 3.0 * maskFactor2,2.0 * maskFactor2, 1.0 * maskFactor2 };


//#define MASK_SIZE 49u
const glm::vec2 texIdx3 = (
	glm::vec2(-3.0, 3.0), glm::vec2(-2.0, 3.0), glm::vec2(-1.0, 3.0), glm::vec2(0.0, 3.0), glm::vec2(1.0, 3.0), glm::vec2(2.0, 3.0), glm::vec2(3.0, 3.0),
	glm::vec2(-3.0, 2.0), glm::vec2(-2.0, 2.0), glm::vec2(-1.0, 2.0), glm::vec2(0.0, 2.0), glm::vec2(1.0, 2.0), glm::vec2(2.0, 2.0), glm::vec2(3.0, 2.0),
	glm::vec2(-3.0, 1.0), glm::vec2(-2.0, 1.0), glm::vec2(-1.0, 1.0), glm::vec2(0.0, 1.0), glm::vec2(1.0, 1.0), glm::vec2(2.0, 1.0), glm::vec2(3.0, 1.0),
	glm::vec2(-3.0, 0.0), glm::vec2(-2.0, 0.0), glm::vec2(-1.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 0.0), glm::vec2(2.0, 0.0), glm::vec2(3.0, 0.0),
	glm::vec2(-3.0, -1.0), glm::vec2(-2.0, -1.0), glm::vec2(-1.0, -1.0), glm::vec2(0.0, -1.0), glm::vec2(1.0, -1.0), glm::vec2(2.0, -1.0), glm::vec2(3.0, -1.0),
	glm::vec2(-3.0, -2.0), glm::vec2(-2.0, -2.0), glm::vec2(-1.0, -2.0), glm::vec2(0.0, -2.0), glm::vec2(1.0, -2.0), glm::vec2(2.0, -2.0), glm::vec2(3.0, -2.0),
	glm::vec2(-3.0, -3.0), glm::vec2(-2.0, -3.0), glm::vec2(-1.0, -3.0), glm::vec2(0.0, -3.0), glm::vec2(1.0, -3.0), glm::vec2(2.0, -3.0), glm::vec2(3.0, -3.0) );
const float maskFactor3 = float (1.0/190.0);
float mask3[] = {
	1.0 * maskFactor3, 2.0 * maskFactor3, 3.0 * maskFactor3, 4.0 * maskFactor3, 3.0 * maskFactor3, 2.0 * maskFactor3, 1.0 * maskFactor3,
	2.0 * maskFactor3, 3.0 * maskFactor3, 4.0 * maskFactor3, 5.0 * maskFactor3, 4.0 * maskFactor3, 3.0 * maskFactor3, 2.0 * maskFactor3,
	3.0 * maskFactor3, 4.0 * maskFactor3, 5.0 * maskFactor3, 6.0 * maskFactor3, 5.0 * maskFactor3, 4.0 * maskFactor3, 3.0 * maskFactor3,
	4.0 * maskFactor3, 5.0 * maskFactor3, 6.0 * maskFactor3, 7.0 * maskFactor3, 6.0 * maskFactor3, 5.0 * maskFactor3, 4.0 * maskFactor3,
	3.0 * maskFactor3, 4.0 * maskFactor3, 5.0 * maskFactor3, 6.0 * maskFactor3, 5.0 * maskFactor3, 4.0 * maskFactor3, 3.0 * maskFactor3,
	2.0 * maskFactor3, 3.0 * maskFactor3, 4.0 * maskFactor3, 5.0 * maskFactor3, 4.0 * maskFactor3, 3.0 * maskFactor3, 2.0 * maskFactor3,
	1.0 * maskFactor3, 2.0 * maskFactor3, 3.0 * maskFactor3, 4.0 * maskFactor3, 3.0 * maskFactor3, 2.0 * maskFactor3, 1.0 * maskFactor3 };*/