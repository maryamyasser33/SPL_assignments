int femtoshell_main(int argc, char *argv[]) {
   while(true){
       printf("FemtoShell >");
       char input[100000];
    if( fgets(input, sizeof(input), stdin)==NULL)
       break; 
        
        input[strcspn(input, "\n")] = 0;
        if(strlen(input)==0)continue;
       if(strncmp(input, "echo ", 5) == 0){
           printf("%s\n",input+5);
       }else if(strncmp(input, "exit", 4) == 0){
           printf("Good Bye\n");
           break;
       }else  printf("Invalid command\n");
   }
   return 0;
}
