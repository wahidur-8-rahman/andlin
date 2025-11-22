#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdatomic.h>

atomic_bool global_stop_signal=false;

void* thread_music(void* arg)
{
  int count=5;

  while(count && !atomic_load(&global_stop_signal))
    {
      system("termux-media-player play /data/data/com.termux/files/home/storage/andlin/Alarm_app/Tones/alarm2.mp3");
      sleep(10);
      system("termux-media-player pause");
      system("termux-media-player stop");
      count--;
    }
  atomic_store(&global_stop_signal, true); //alarm played 5 times, signal other threads about it.
  return NULL;
}
void* thread_vibrate(void* arg)
{
  int vibrate_limiter=50; //indepenedent stopper for emergency
  while(!atomic_load(&global_stop_signal) && vibrate_limiter)
    {
      system("termux-vibrate -d 750 -f");
      sleep(1); //250 ms of break
      vibrate_limiter--;
    }
  atomic_store(&global_stop_signal, true);//signal other threads to stop in case of independent stop



  return NULL;
}
void* thread_check(void* arg)
{
  while(!global_stop_signal)
    {
      system("termux-battery-status > bat_stat.json");
      usleep(100000); // to give some time to execute and create file 0.1 sec
      system("python bat_stat_modifier.py");
      usleep(100000);//to give some time to modify file 0.1 sec

      FILE* fp=fopen("bat_stat.txt", "r");

      if(!fp) continue;

      char line[20];

      fgets(line, sizeof(line), fp);
      fclose(fp);
      if(strcmp(line, "CHARGING")==0)
	{
	  atomic_store(&global_stop_signal,true);
	  break;
	}

    }
  return NULL;
}

int main(int argc, char* argv[])
{
  if(argc!=3)
    {
      printf("Usage %s <minutes> <seconds>\n", argv[0]);
      return 1;
    }
  
  pthread_t tid1, tid2, tid3;

  int min_user=atoi(argv[1]);
  int sec_user=atoi(argv[2]);


  /* time_t t=time(NULL);

  struct tm *tm_info=localtime(&t);

  int hour_curr=tm_info->tm_hour;
  int min_curr=tm_info->tm_min;
  

  int hour_diff=hour_user - hour_curr;
  int min_diff = min_user - min_curr;


  if(hour_diff < 0) // hour in -ve
    hour_diff=hour_diff + 24;

  if(min_diff < 0) // min in -ve
    {
      min_diff=min_diff + 60;
      hour_diff--;
    }
  */
  int time_diff_in_sec=min_user*60 + sec_user;

  printf("Timer will ruin your... whatever tf you're doing in %d minute(s) and %d second(s)\n", min_user, sec_user);

  sleep(time_diff_in_sec);

  /* thread 1 plays music
          thread 2 vibrates :-)
	       thread 3 checks condition
  */
  pthread_create(&tid1, NULL, &thread_music, NULL);
  pthread_create(&tid2, NULL, &thread_vibrate, NULL);
  pthread_create(&tid3, NULL, &thread_check, NULL);

  pthread_join(tid3, NULL); //wait for thread_condition function first
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  return 0;
}
