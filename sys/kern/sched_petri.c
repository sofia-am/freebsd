#include <sys/sched_petri.h>

/*
GLOBAL VARIABLES
*/
const int matrix_Incidence[PLACES_SIZE][TRANSITIONS_SIZE] = {
	{-1,  0,  0,  0,  0,  0 ,  0}, //inactive
	{ 1, -1,  0,  1,  0,  1 ,  1}, //can_run
	{ 0,  1, -1,  0,  0,  0 , -1}, //runq
	{ 0,  0,  1, -1, -1,  0 ,  0}, //running
	{ 0,  0,  0,  0,  1, -1 ,  0}  //inhibited
};

const int initial_mark[PLACES_SIZE] = { 0, 1, 0, 0, 0 };

__inline int
thread_transition_is_sensitized(struct thread *pt, int transition_index);

/**
 * @brief esta funcion recibe un thread como parametro y procede a 
 * inicializar su mark al marcado inicial
 * 
 * @param pt_thread 
 */
void
init_petri_thread(struct thread *pt_thread){ 
	// Create a new petr_thread structure
	int i;
	for (i = 0; i < PLACES_SIZE; i++) {
		pt_thread->mark[i] = initial_mark[i];
	}//mark: vector que tiene el marcado inicial, con tamaño PLACES SIZE.
	pt_thread->td_frominh = 0; // (flag) Thread comes from an inhibited state
}

/**
 * @brief recibe un thread como parámetro y analiza todas sus 
 * transiciones para actualizar su sensitized_buffer
(función no utilizada).
 * 
 * @param pt 
 */
void
thread_get_sensitized(struct thread *pt)
{
	int k;
	for(k=0; k< TRANSITIONS_SIZE; k++){
		if(thread_transition_is_sensitized(pt, k))
			pt->sensitized_buffer[k] = 1; 
/* sensitized_buffer: vector que representa las transiciones sensibili-
zadas de su red asociada, con tamaño TRANSITIONS SIZE.*/
		else
			pt->sensitized_buffer[k] = 0;
	}
};
/**
 * @brief 
 * 
 * @param pt 
 * @param transition_index 
 * @return __inline 
 */
__inline int
thread_transition_is_sensitized(struct thread *pt, int transition_index)
{
	int places_index;

	for (places_index = 0; places_index < PLACES_SIZE; places_index++) {

		if (((matrix_Incidence[places_index][transition_index] < 0) && 
			//If incidence is positive we really dont care if there are tokens or not
			((matrix_Incidence[places_index][transition_index] + pt->mark[places_index]) < 0)))
		{
			return 0;
		}
	}
	return 1;
}

/**
 * @brief recibe un thread y una transición como paráme-
tros y la dispara haciendo uso de la matriz de incidencia, actualizando
finalmente su marcado.
 * 
 * @param pt 
 * @param transition 
 */
void
thread_petri_fire(struct thread *pt, int transition)
{
	int i;
	if(thread_transition_is_sensitized(pt, transition)){
		for(i=0; i < PLACES_SIZE; i++)
			pt->mark[i] += matrix_Incidence[i][transition];
	}
	else
	{
		printf("Transition %2d no estaba sensibilizada para thread %d\n", transition, pt->td_tid);
		thread_print_net(pt);
		print_resource_net();	
	}
}


/*This method is not used yet */

/**
 * @brief recibe un thread como parámetro y calcula sus transiciones 
 * sensibilizadas para proceder a dispararlas (función
no utilizada).
 * 
 * @param pt 
 */
static void
thread_search_and_fire(struct thread *pt){
	int i;
	thread_get_sensitized(pt);
	i=0;
	while((pt->sensitized_buffer[i] != 1) && (i < TRANSITIONS_SIZE)){
		i++;
	}
	if(i < TRANSITIONS_SIZE){
		thread_petri_fire(pt, i);
	}
}

void thread_print_net(struct thread *pt)
{
	int i;
	printf("Marcado thread %d: ", pt->td_tid);
	for(i=0; i< PLACES_SIZE; i++){
		printf("%d ", pt->mark[i]);
	}
	printf("\n");
}