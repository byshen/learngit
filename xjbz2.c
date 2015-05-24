
#include "cse356header.h"

sem_t clk_mem,clk_sch,se,mem_sch;//protect sem!!!     initial   se ÊÇÉ¶

struct process
{
int id;
char*name;
int CPU_time,arrive_time,finish_time,page_faults;
int memory_trace[100];
int memory_trace_i;
};
struct timeval startTime, endTime;
struct timezone tz;
struct tm *tm;

int mem_read_queue[300];
int start=0,end=0;
int pages,quantum;
int f_block;
int length=300;
long context_switch=0,page_faults=0,cnt=0,clk_unused=0;
int total_process,total_process2;
struct process *blocked_queue[300];
int start_bq=0,end_bq=0;
struct process *ready_queue[300];
int start_rq=0,end_rq=0;
struct process process_queue[200];//??shu liang geng duo?
long current_time=0;
void *schduler_timer();
void *memory_FIFO();
void *memory_LRU();
void *memory_SECONDCHANCE();
void *schduler();

main ( int argc, char *argv[] )
{

pages=atoi(argv[1]);
quantum=atoi(argv[2]);
char buf[400],buf_pt[600];

FILE *trace_file,*process_trace;


trace_file = fopen (argv[4],"r");
if (trace_file == NULL) {
printf ("Error: Could not open file '%s'.\n", argv[4]);
exit(-1);
}

int i_process=0;

char *p;
int cnt_buf;
cnt_buf=fread(buf,1,400,trace_file);
buf[cnt_buf]='\0';
fclose(trace_file);

p = strtok(buf, " ");
do{
process_queue[i_process].name=p;
process_queue[i_process].memory_trace_i=0;
process_queue[i_process].page_faults=0;


process_trace = fopen (p,"r");
if (process_trace == NULL) {
printf ("Error: Could not open file '%s'.\n", p);
exit(-1);
}
fread(buf_pt,1,600,process_trace);
fclose(process_trace);
int i_process_trace=0;
int i_process_trace_cnt=0;
int num_trace=0;
int i_t=0;
while(buf_pt[i_process_trace_cnt])
{
    if(buf_pt[i_process_trace_cnt]=='x')
    {
        num_trace=0;
        for(i_t=0;i_t<5;i_t++)
       {
        num_trace*=16;
        if(buf_pt[i_process_trace_cnt+i_t+1]>='0'&&buf_pt[i_process_trace_cnt+1]<='9')
            num_trace+=buf_pt[i_process_trace_cnt+i_t+1]-'0';
        else
            num_trace+=buf_pt[i_process_trace_cnt+i_t+1]-'a'+10;
        }
        process_queue[i_process].memory_trace[i_process_trace++]=num_trace;
        i_process_trace_cnt+=4;


    }
    i_process_trace_cnt++;
}
p = strtok(NULL," ");
 process_queue[i_process].memory_trace[i_process_trace]=-1;

process_queue[i_process].arrive_time=(int)(atof(p)*100000);
printf("%d\n",process_queue[i_process].arrive_time);
p = strtok(NULL," ");
process_queue[i_process].CPU_time=(int)(atof(p)*100000);
printf("%d\n",process_queue[i_process].CPU_time);
p = strtok(NULL," ");
process_queue[i_process].id=i_process++;

while(p[0]<='9'&&p[0]>='0')p++;

if(p[0]=='\n')
p++;
else
p=0;
//printf("%s\n",p);
}while(*p);// zui hou i=num+1
total_process=total_process2=i_process;




sem_init(&clk_mem,0,0);//initial semaphore
sem_init(&clk_sch,0,0);
sem_init(&mem_sch,0,0);
sem_init(&se,0,1);



pthread_t t_mem,t_sch,t_clk;
pthread_attr_t attr1;
pthread_attr_init(&attr1);
pthread_attr_setscope(&attr1, PTHREAD_SCOPE_SYSTEM);
pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);

int rc_mem ;//allocate space
int rc_sch ;
int rc_clk ;


if(argv[3][0]=='F')
{
rc_mem = pthread_create (&t_mem, &attr1,memory_FIFO, 0);
printf("F\n");
}
else if(argv[3][0]=='L')
{
rc_mem = pthread_create (&t_mem, &attr1,memory_LRU, 0);
printf("L\n");
}
else
{
rc_mem = pthread_create (&t_mem, &attr1,memory_SECONDCHANCE, 0);
printf("S\n");
}
//create thread
if (rc_mem) {
printf("ERROR: return code from pthread_create(t1) is %d\n", rc_mem);
exit(-1);}



rc_sch = pthread_create (&t_sch, &attr1,schduler, 0);
//create thread
if (rc_sch) {
printf("ERROR: return code from pthread_create(t1) is %d\n", rc_sch);
exit(-1);}

rc_clk = pthread_create (&t_clk, &attr1,schduler_timer, 0);
//create thread
if (rc_clk) {
printf("ERROR: return code from pthread_create(t1) is %d\n", rc_clk);
exit(-1);}





void *status;
/*
rc_mem = pthread_join (t_clk, &status);
if (rc_clk)
{
printf("ERROR: return code from pthread_join(t1) is %d\n", rc_clk);
exit(-1);
}


rc_mem = pthread_join (t_mem, &status);
if (rc_mem)
{
printf("ERROR: return code from pthread_join(t1) is %d\n", rc_mem);
exit(-1);
}
*/
rc_sch = pthread_join (t_sch, &status);
if (rc_sch)
{
printf("ERROR: return code from pthread_join(t1) is %d\n", rc_sch);
exit(-1);
}

int disp_i=0;
long final_time=0;
for(disp_i=0;disp_i<total_process;disp_i++)
{
    printf("Process : %s\n",process_queue[disp_i].name);
    printf("Elapsed time : %d\n",process_queue[disp_i].finish_time-process_queue[disp_i].arrive_time);
    printf("Page faults : %d\n",process_queue[disp_i].page_faults);
    if(final_time<process_queue[disp_i].finish_time)
        final_time=process_queue[disp_i].finish_time;
}
printf("Total elapsed time : %ld\n",final_time);
printf("Total page faults : %ld\n",page_faults);
printf("Total idle time : %ld\n",clk_unused);

sem_destroy(&clk_mem);
sem_destroy(&clk_sch);
sem_destroy(&se);
return 0;

}


void *memory_FIFO(){//can read while write?
//struct id *a;
//a = (struct id *)args;

int main_mem[pages];
int v_i[pages];
int disk_to_mem[300];
int start_dtm=0,end_dtm=0;
int flag;
int oldest_page=0;



int i=0;
for(i=0;i<pages;i++)
v_i[i]=0;

int clk_count=0;
while(1)
{
sem_wait(&clk_mem);
sem_wait(&se);

while(start!=end)
{
flag=0;
for(i=0;i<pages;i++)
if(main_mem[i]==mem_read_queue[start]&&v_i[i])
{flag=1;
printf("hit%d,%ld\n",mem_read_queue[start],current_time);
break;
}
if(flag==0)
{

printf("pagefault%d,%ld\n",mem_read_queue[start],current_time);
page_faults++;
disk_to_mem[end_dtm]=mem_read_queue[start];
end_dtm=(end_dtm+1)%300;
f_block=1;
//disable a mem space by -1
}
start++;
}
sem_post(&se);

if(start_dtm!=end_dtm)
{
v_i[oldest_page]=0;
if(clk_count!=999)
clk_count++;
else
{
//mou zhong ti huan fang shi geng xin memspace
printf("newpage%ld\n",current_time);
v_i[oldest_page]=1;
main_mem[oldest_page]=disk_to_mem[start_dtm];
ready_queue[end_rq]=blocked_queue[start_bq];
start_bq=(1+start_bq)%length;
end_rq=(1+end_rq)%length;
oldest_page=(1+oldest_page)%pages;
start_dtm=(start_dtm+1)%300;
clk_count=0;
}
}

sem_post(&mem_sch);
}
}


void *memory_LRU(){//can read while write?
//struct id *a;
//a = (struct id *)args;

int main_mem[pages];
int v_i[pages];
int age[pages];
int disk_to_mem[300];
int start_dtm=0,end_dtm=0;
int flag;
int age_now=0;
int min_age,index_min_age;
int search_flag=0;

int i=0;
for(i=0;i<pages;i++)
{
    v_i[i]=0;
    age[i]=-1;
}
int clk_count=0;
while(1)
{
sem_wait(&clk_mem);
sem_wait(&se);
while(start!=end)
{
flag=0;
for(i=0;i<pages;i++)
if(main_mem[i]==mem_read_queue[start]&&v_i[i])
{flag=1;
printf("hit%d,%ld\n",mem_read_queue[start],current_time);
age[i]=age_now++;
break;
}
if(flag==0)
{


printf("pagefault%d,%ld\n",mem_read_queue[start],current_time);
page_faults++;
disk_to_mem[end_dtm]=mem_read_queue[start];
end_dtm=(end_dtm+1)%300;
f_block=1;
if(end_dtm-start_dtm==1)
    search_flag=1;
}
start++;
}
sem_post(&se);

if(start_dtm!=end_dtm)
{
    if(search_flag)
    {
    search_flag=0;
    min_age=1000000;
    for(i=0;i<pages;i++)
    {
    if(min_age>age[i])
    {
        min_age=age[i];
        index_min_age=i;
    }
    }
    v_i[index_min_age]=0;
    }



if(clk_count!=999)
clk_count++;
else
{
//mou zhong ti huan fang shi geng xin memspace
	printf("newpage%ld\n",current_time);
	main_mem[index_min_age]=disk_to_mem[start_dtm];
	age[index_min_age]=age_now++;
	v_i[index_min_age]=1;
	ready_queue[end_rq]=blocked_queue[start_bq];
	start_bq=(1+start_bq)%length;
	end_rq=(1+end_rq)%length;
	start_dtm=(start_dtm+1)%300;
	clk_count=0;
	if(start_dtm!=end_dtm)
	    search_flag=1;
	}
}
sem_post(&mem_sch);
}
}


void *memory_SECONDCHANCE(){//can read while write?
//struct id *a;
//a = (struct id *)args;

int main_mem[pages];
int v_i[pages];
int disk_to_mem[300];
int start_dtm=0,end_dtm=0;
int flag;
int chance[pages];
int pointer=0;
int search_flag=0;


int i=0;
for(i=0;i<pages;i++)
{v_i[i]=0;
chance[i]=0;
}
int clk_count=0;
while(1)
{
sem_wait(&clk_mem);
sem_wait(&se);
while(start!=end)
{
flag=0;
for(i=0;i<pages;i++)
if(main_mem[i]==mem_read_queue[start]&&v_i[i])
{flag=1;
printf("hit%d,%ld\n",mem_read_queue[start],current_time);
chance[i]=1;
break;
}
if(flag==0)
{

printf("pagefault%d,%ld\n",mem_read_queue[start],current_time);
page_faults++;
disk_to_mem[end_dtm]=mem_read_queue[start];
end_dtm=(end_dtm+1)%300;
f_block=1;
if(end_dtm-start_dtm==1)
    search_flag=1;
//disable a mem space by -1
}
start++;
}
sem_post(&se);

if(start_dtm!=end_dtm)
{
    if(search_flag)
    {
    search_flag=0;
    while(chance[pointer])
        {
            chance[pointer]=0;
            pointer=(1+pointer)%pages;
        }
    v_i[pointer]=0;
    }
if(clk_count!=999)
clk_count++;
else
{
//mou zhong ti huan fang shi geng xin memspace
printf("newpage%ld\n",current_time);
v_i[pointer]=1;
main_mem[pointer]=disk_to_mem[start_dtm];
ready_queue[end_rq]=blocked_queue[start_bq];
start_bq=(1+start_bq)%length;
end_rq=(1+end_rq)%length;
pointer++;
start_dtm=(start_dtm+1)%300;
clk_count=0;
if(start_dtm!=end_dtm)
    search_flag=1;
}
}
sem_post(&mem_sch);
}
}



void *schduler(){
struct process *running;
long clk_count=0;
int f_switch=0,f_running=0;
f_block=0;
sem_post(&mem_sch);
while(total_process2)
{
sem_wait(&clk_sch);
sem_wait(&mem_sch);

current_time++;
if(f_block)
{
    printf("block%ld\n",current_time);
    running->page_faults++;
    f_block=0;
    blocked_queue[end_bq]=running;
    end_bq=(1+end_bq)%length;
    f_running=0;
    if(start_rq!=end_rq)
    {
        clk_count=0;
        f_switch=1;
        running=ready_queue[start_rq];
        start_rq=(1+start_rq)%length;
    }
    else
        clk_unused++;
}
else
if(f_switch)// xin pocess yi ing zai rnning li le
if(clk_count>=49)
{printf("switch%ld\n",current_time);f_switch=0;clk_count=0;f_running=1;}
else
{clk_count++;}
else
{
if(f_running)
{
//printf("running%ld\n",cnt);
//printf("%s\n",running->name);
if(clk_count<quantum-1&&running->CPU_time>1)
{
    clk_count++;running->CPU_time--;
    if(running->memory_trace[running->memory_trace_i]!=-1)
    {
        sem_wait(&se);
        mem_read_queue[end]=running->memory_trace[running->memory_trace_i];
        end=(1+end)%length;
        running->memory_trace_i++;
        sem_post(&se);
    }
    //printf("A");
}
else if(clk_count==quantum-1&&running->CPU_time>1)
{
printf("context_switch%ld\n",current_time);
printf("left time%d\n",running->CPU_time);
context_switch++;
ready_queue[end_rq]=running;
end_rq=(1+end_rq)%length;
running=ready_queue[start_rq];
start_rq=(1+start_rq)%length;
clk_count=0;
f_switch=1;
f_running=0;
}
else
{
f_running=0;
printf("finish%ld\n",current_time);
printf("%s\n",running->name);
running->finish_time=current_time;
total_process2--;
if(start_rq!=end_rq)
{
clk_count=0;
f_switch=1;
running=ready_queue[start_rq];
start_rq=(1+start_rq)%length;
}
}
}
else
if(start_rq!=end_rq)
{

    f_switch=1;
    clk_count=0;
    running=ready_queue[start_rq];
    start_rq=(1+start_rq)%length;
}
else
    clk_unused++;
}

sem_post(&clk_mem);
}
}







void *schduler_timer()
{
   long run_time_in_microseconds;
   clock_t start, end;
   double elapsed;
   gettimeofday(&startTime, &tz);
   long i,ii,cnt_i=0;
   int process_to_arrive=0;
   int flag=1;
   while(1)
   {
       gettimeofday(&endTime, &tz);
       startTime.tv_usec=0;
       run_time_in_microseconds = endTime.tv_usec - startTime.tv_usec;

       //startTime.tv_usec=endTime.tv_usec;
       i=run_time_in_microseconds/10-cnt_i;
       if(i<0)
       {
       i=0;cnt_i=0;
       }
       for(ii=0;ii<i;ii++)
       {

           sem_post(&clk_sch);
       }
       cnt=cnt+i;
       cnt_i+=i;
       //printf("%ld\n",run_time_in_microseconds);
	//if(i<0)
       //printf("%ld\n",cnt);

       while(process_queue[process_to_arrive].arrive_time<=cnt&&process_to_arrive<total_process)
       {
          ready_queue[end_rq]=&process_queue[process_to_arrive];
          end_rq=(1+end_rq)%length;
          process_to_arrive++;
          printf("%d\n",process_to_arrive);
       }
   }
}




