/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "buzzer.h"

#define GREEN_LED BIT6


AbRect rect10 = {abRectGetBounds, abRectCheck, {10,10}}; /**< 10x10 rectangle */
AbRect bar = {abRectGetBounds, abRectCheck, {10,3}};
AbRArrow rightArrow = {abRArrowGetBounds, abRArrowCheck, 10};

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 1, screenHeight/2 - 7}
};


Layer layer15 = {
  (AbShape *)&circle5,
  {(screenWidth/2)+50, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  0
};

Layer layer14 = {
  (AbShape *)&circle5,
  {(screenWidth/2)+40, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer15,
};

Layer layer13 = {
  (AbShape *)&circle5,
  {(screenWidth/2)+30, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer14,
};

Layer layer12 = {
  (AbShape *)&circle5,
  {(screenWidth/2)+20, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer13,
};

Layer layer11 = {
  (AbShape *)&circle5,
  {(screenWidth/2)+10, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer12,
};

Layer layer10 = {
  (AbShape *)&circle5,
  {(screenWidth/2), (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer11,
};




Layer layer9 = {
  (AbShape *)&circle5,
  {(screenWidth/2)-10, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer10,
};


Layer layer8 = {
  (AbShape *)&circle5,
  {(screenWidth/2)-20, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer9
};

Layer layer7 = {
  (AbShape *)&circle5,
  {(screenWidth/2)-30, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer8,
};


Layer layer6 = {
  (AbShape *)&circle5,
  {(screenWidth/2)-40, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer7,
};

Layer layer5 = {
  (AbShape *)&circle5,
  {(screenWidth/2)-50, (screenHeight/2)-60}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_GREEN,
  &layer6,
};

Layer layer4 = {
  (AbShape *)&circle4,
  {(screenWidth/2)+10, (screenHeight/2)+5}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_VIOLET,
  &layer5,
};


//Layer layer4 = {
//  (AbShape *)&rightArrow,
//  {(screenWidth/2)+10, (screenHeight/2)+64}, /**< bit below & right of center */
//  {0,0}, {0,0},				    /* last & next pos */
//  COLOR_RED,
//  0
//};
  
Layer layer3 = {		/**< Layer with a red square */
  (AbShape *)&circle4,
  {screenWidth/2, (screenHeight/2)+10}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_ORANGE,
  
  //&layer4,
  //&fieldLayer,
};



Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  // &layer3,
  0
};




Layer layer1 = {		/**< Layer with an orange circle */
  (AbShape *)&circle3,
  {(screenWidth/2)+10, (screenHeight/2)-50}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};

Layer layer0 = {		/**< Layer with an orange circle */
  (AbShape *)&bar,
  {(screenWidth/2)+10, (screenHeight/2)+65}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &layer1,
};

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */

//MovLayer ml4 = { &layer3, {5,5}, 0 }; /**< not all layers move */
MovLayer ml3 = { &layer0, {8,0}, 0 };
MovLayer ml2 = { &layer1, {6,-4}, 0 };
//MovLayer ml1 = { &layer0, {5,5}, 0 };



//MovLayer ml0 = { &layer0, {10,10}, &ml1 };
//MovLayer ml4 = { &layer4, {5,5}, 0};


movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  



//Region fence = {{0, LONG_EDGE_PIXELS}, {SHORT_EDGE_PIXELS-1, LONG_EDGE_PIXELS}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */

void mlAdvanceP1(MovLayer *ml, MovLayer *ml1, Region *fence){
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
      }	/**< if outside of fence */
      if(collisionDetector(&ml3, &ml2))
	{
      	  int velocity1 = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
      	  newPos.axes[axis] += (2*velocity1);
      	}
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}

void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
      }	/**< if outside of fence */
      if(collisionDetector(&ml3, &ml2)){
	int velocity1 = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity1);
      }
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}


u_int bgColor = COLOR_BLACK;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */

int playerScore = 0;

void score(MovLayer *ml, Region *fence){
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    if(collisionDetector(&ml2, &ml3)) {
       playerScore += 1;
      }
    if(shapeBoundary.botRight.axes[1] > fence->botRight.axes[1]) {
      	buzzer_set_period(10000);
	playerScore -= 1;

    }
  } /**< for ml */
}

int collisionDetector(const MovLayer *ml, const MovLayer *bar){
  Region ball;
  Region br;
  Region detector;

  abShapeGetBounds(ml->layer->abShape, &(ml->layer->pos), &ball);
  abShapeGetBounds(bar->layer->abShape, &(bar->layer->pos), &br);
  intersectionDetector(&detector, &ball, &br);  
  
  int x1 = detector.topLeft.axes[0];
  int x2 = detector.botRight.axes[0];
  int y1 = detector.topLeft.axes[1];
  int y2 = detector.botRight.axes[1];
  
  if(x1 >= x2 || y1 >= y2)
    {
      return 0;
    }
  return 1;
}

void intersectionDetector(Region *detector, const Region *r1, const Region *r2){
  vec2Max(&detector->topLeft, &r1->topLeft, &r2->topLeft);
  vec2Min(&detector->botRight, &r1->botRight, &r2->botRight);
}

void sound(){
  if(collisionDetector(&ml3, &ml2)){
      buzzer_set_period(1000);
    }
  if(!(collisionDetector(&ml3, &ml2))){
    //buzzer_set_period(10000);
    }
}

/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(1);

  shapeInit();

  layerInit(&layer0);
  layerDraw(&layer0);

  layerGetBounds(&fieldLayer, &fieldFence);
  u_char width = screenWidth, height = screenHeight;


  fillRectangle(0,0, 130, 7, COLOR_RED);
  //fillRectangle(140, 0, 0, 50, COLOR_YELLOW);
  drawString5x7(5,0, "SCORE", COLOR_WHITE, COLOR_RED);
  
  fillRectangle(0,155, 130, 5, COLOR_BLUE);

  enableWDTInterrupts();      /**< enable periodic interrupt */
  buzzer_init();
  or_sr(0x8);	              /**< GIE (enable interrupts) */


  for(;;) {
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    
    int score[1];
    score[0] = '0' + playerScore;
    drawString5x7(screenWidth/2,0, score, COLOR_WHITE, COLOR_RED);
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    //movLayerDraw(&ml1, &layer0);
    movLayerDraw(&ml2, &layer0);
    movLayerDraw(&ml3, &layer0);
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 15) {
    mlAdvanceP1(&ml2, &ml3, &fieldFence);
    score(&ml2, &fieldFence);
    //mlAdvance(&ml2, &fieldFence);
    //mlAdvance(&ml3, &fieldFence);
    //mlAdvance1(&ml1, &fence);
    //mlAdvance1(&ml4, &fence);
    buzzer_set_period(0);
    sound();

    /* if(playerScore == 1){
      MovLayer ml2 = { &layer1, {-1,7}, 0 };
      movLayerDraw(&ml4, &layer0);
      //      buzzer_set_period(5000);
      }*/
    /* if(playerScore == ){
      buzzer_set_period(10000);
      drawString5x7(screenWidth/2,screenHeight/2, "YOU LOST", COLOR_WHITE, COLOR_RED);
      }
    if(playerScore == 1){
      MovLayer ml3 = { &layer0, {1,0}, 0 };
      buzzer_set_period(3000);
    }
    */
    

    if(playerScore == 5){
      buzzer_set_period(5000);
      drawString5x7(screenWidth/2,screenHeight/2, "YOU WON", COLOR_WHITE, COLOR_BLACK);
    }
    if (!(p2sw_read() & BIT0)){
      //mlAdvanceP1(&ml3, &ml2, &fieldFence);
      mlAdvance(&ml3, &fieldFence);
      //mlAdvance(&ml3, &fieldFence);
    }
    redrawScreen = 1;
    
    count = 0;
  }
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
