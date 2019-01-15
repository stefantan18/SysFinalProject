#include "networking.h"

int getMajority(int vote[], int size) {
  int current = 0;
  for(int i = 0; i < size;i++){
    if(vote[i] > vote[current]){
      current = i;
      }
    }
  return current;
}

int main() {

  int server_socket;
  char buffer[BUFFER_SIZE];

  //Setting up the Game
  int num_players = 0;

  char *host = malloc(2);
  printf("Are you hosting or connecting? [y/n]: ");
  fgets(host, 2, stdin);
  
  if(strcmp(host, "y") == 0) {
    printf("Enter the number of players(6-10 players Only!): ");
    fgets(buffer, sizeof(buffer), stdin);
    num_players = atoi(buffer);
    *strchr(buffer, '\n') = 0;
    server_socket = client_setup(TEST_IP);
    write(server_socket, buffer, sizeof(buffer));
    
    //creating number of players to shmem for subservers to access
    //subservers are supposed to increment this when they connect
    int key = 99;
    int shmid = shmget(key, sizeof(int), IPC_CREAT);
    int *num =(int *) shmat(shmid, NULL, 0);
    *num = 0;
    shmdt(num);    
  }
  else {
    char *ip = malloc(20);
    printf("Please enter the ip address for the host server: ");
    fgets(ip, 20, stdin);
    server_socket = client_setup(ip);
  }

  int notalive[num_players];//if players are alive
  int numDead = 0; //number of dead players
  int recently_killed = 0; //stores the most recent death
  int votes[num_players]; //store the votes
  int your_choice = 0; //placed into vote

  //Receiving roles
  read(server_socket, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  for (int i = 0; i < num_players; i++) {
    printf("Welcome Player_%d!\n", i);
    printf("To confirm that you are Player_%d, please enter 'y': ", i);
    fgets(buffer, sizeof(buffer), stdin);
    if (!strcmp(buffer, "y\n")) {
      *strchr(buffer, '\n') = 0;
      write(server_socket, buffer, sizeof(buffer));
      read(server_socket, buffer, sizeof(buffer));
      printf("\nCONFIDENTIAL INFO!\n\n %s\n\n", buffer);
      printf("Enter 'y' if understood: ");
      fgets(buffer, sizeof(buffer), stdin);
      if (!strcmp(buffer, "y\n")) {
        execlp("clear", "clear", NULL);
	      printf("Switch to the next player.");
      }
    }
  }

  printf("Enter 'y' if you are ready to start mafia?");
  fgets(buffer, sizeof(buffer), stdin);
  if (!strcmp(buffer, "y\n")) {
    int i = 0;
    while(i < num_players) {
      printf("Welcome Player_%d!\n", i);
      printf("To confirm that you are Player_%d, please enter 'y': ", i);
      fgets(buffer, sizeof(buffer), stdin);
      if (!strcmp(buffer, "y\n")) {
        sprintf(buffer, "%d", i);
        *strchr(buffer, '\n') = 0;
        write(server_socket, buffer, sizeof(buffer));
        read(server_socket, buffer, sizeof(buffer));
        printf("%s\n", buffer);
        if (!strcmp(buffer, "It's nighttime in the community. Time to eliminate a civilian!")) {
          printf("Select your target (0-%d): ", num_players - 1);
          fgets(buffer, sizeof(buffer), stdin);
          recently_killed = atoi(buffer);
          notalive[numDead] = recently_killed;
          numDead++;
          *strchr(buffer, '\n') = 0;
          write(server_socket, buffer, sizeof(buffer));
          if (numDead == num_players - 2) {
            break;
          }
        }
        else if (strcmp(buffer, "It's nighttime in the community and you are currently sleeping.")) {
          printf("Let the vote commence! Who do you believe is the mafia member? (0-%d)", num_players);
          fgets(buffer, sizeof(buffer), stdin);
          your_choice = atoi(buffer);
          votes[your_choice] += 1;

          if (i == num_players - 1) {
            printf("Enter 'y' if done: ");
            fgets(buffer, sizeof(buffer), stdin);
            if (!strcmp(buffer, "y\n")) {
              execlp("clear", "clear", NULL);
              printf("All players can see this!\n");
              printf("The vote has concluded.\n");
              printf("The majority has decided that Player_%d should be executed.\n", getMajority(votes,num_players));
              printf("Player_%d has been executed.\n", getMajority(votes,num_players));
              sprintf(buffer, "%d", getMajority(votes,num_players));
              *strchr(buffer, '\n') = 0;
              write(server_socket, buffer, sizeof(buffer));
              read(server_socket, buffer, sizeof(buffer));
              printf("%s\n", buffer);
              if (!strcmp(buffer, "THE MAFIA HAS BEEN EXECUTED!")) {
                break;
              }
            }
          }
        }
        printf("Enter 'y' if done: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (!strcmp(buffer, "y\n")) {
          execlp("clear", "clear", NULL);
  	      printf("Switch to the next player.");
          i++;
          if (i == num_players) {
            i = 0;
          }
        }
      }
    }
  }

  execlp("clear", "clear", NULL);
  read(server_socket, buffer, sizeof(buffer));
  printf("%s\n", buffer);
  printf("\nTHANKS FOR PLAYING!\n");
}

  /*
  while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    write(server_socket, buffer, sizeof(buffer));
    read(server_socket, buffer, sizeof(buffer));
    printf("received: [%s]\n", buffer);
    }*/
