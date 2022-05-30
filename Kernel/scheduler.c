#include <scheduler.h>
#include <interrupts.h>
#include <schedulerfunctions.h>

static task processes[NUM_PROCESSES];
static int process_qty = 0;
static int current_pid = -1;
static char scheduler_active = 0;

#define ACTIVE 2
#define IDLE 1
#define PAUSED 0
#define FINISHED -1

void prueba(){}

// Cuando llamamos al pipe se tiene que ejecutar la siguiente funcion;
//Función que crea una task y la agrega al array processes
//devuelve el pid
int load_processes(uint64_t rip, int fd, char * string) {
    
    // if(process_qty < 2){
    //     processes[process_qty++] = new_task;
    //     return process_qty - 1;
    // }
    prueba();
    int found = 0;
    int i;
    for(i = 2; i < process_qty && !found; i++){
        if(processes[i].active == FINISHED){
            found = 1;
        }
    }
    if(!found){
        task new_task = {
            .rip = rip,
            .rsp = initialize_process(STACK_BASE + PROCESS_SIZE * process_qty, rip,fd,string),
            .stack_base = STACK_BASE + PROCESS_SIZE * process_qty,
            .active = IDLE
        };
        prueba();
        processes[process_qty++] = new_task;
        return process_qty - 1;
    }
    task new_task = {
        .rip = rip,
        .rsp = initialize_process(STACK_BASE + PROCESS_SIZE * i, rip,fd,string),
        .stack_base = STACK_BASE + PROCESS_SIZE * i,
        .active = IDLE
    };
    processes[i - 1] = new_task;
    return i - 1;
}

void current_process_returned() {
    task current_task = processes[current_pid];
    current_task.active = FINISHED;
    processes[current_pid] = current_task;
    current_pid = -1;
    scheduler();
    process_qty--;
}

//Guarda la current task a ejecutar, teniendo en cuenta el pid que recibe como argumento
//actualiza el current pid y corre el proceso
//Si terminó de correr el proceso el lugar en el array que guarda los procesos eliminado
//y actualizar el valor de current pid a -1 para saber que no se está corriendo nada
void exec_process(int pid){
    if ( pid < 0 || pid >= process_qty) {
        return;
    }
    task current_task = processes[pid];
    current_pid = pid;
    run_process(current_task.rsp);
    //Si llego aca, es porque el proceso terminó
    current_task.active = FINISHED;
    processes[pid] = current_task;
    current_pid = -1;
}

//Obtiene el proceso de acuerdo al pid recibido por argumento
//y realiza la actualizacion de RIP y RSP de la task
void pause_process(int pid) {
    if ( pid < 0 || pid >= process_qty ) {
        return;
    }
    task current_task = processes[pid];
    current_task.rip = get_current_rip();
    current_task.rsp = stop_process(get_current_rsp(), current_task.rip, get_current_gp_registers());
    current_task.active = IDLE;
    processes[pid] = current_task;
}

// void activate_scheduler() {
//     scheduler_active = 1;
// }

// char is_scheduler_active() {
//     return scheduler_active;
// }

int hibernate_process(int pid){
    if ( pid < 0 || pid >= process_qty ) {
        return 0;
    }
    task current_task = processes[pid];
    current_task.active = PAUSED;
    processes[pid] = current_task;
    return 1;
}

void scheduler(){
    if(process_qty == 0){
        return;
    }
    pause_process(current_pid);
    int next_pid_to_run = (current_pid + 1) % process_qty;
    while (processes[next_pid_to_run].active == FINISHED || processes[next_pid_to_run].active == PAUSED) {
        next_pid_to_run = (next_pid_to_run + 1) % process_qty;
    }
    exec_process(next_pid_to_run);
}