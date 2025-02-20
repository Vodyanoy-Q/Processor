#ifndef _ASSERT_H_
#define _ASSERT_H_


#define MY_ASSERT(addres) if(!addres)                                                        \
                          {                                                                  \
                              printf(" ERROR\n line %d\n file %s\n func %s\n",               \
                                     __LINE__, __FILE__, __func__);                          \
                                                                                             \
                              exit (ADDRES_ERROR);                                           \
                          }                                                                  \

#define _FOPEN(addres, name, arg) addres = fopen(name, arg);                                 \
                                                                                             \
                                  if(addres == 0)                                            \
                                  {                                                          \
                                      printf("ERROR OPEN FILE\n"                             \
                                             "PLEASE CHECK FILE AND TRY AGAIN\n");           \
                                                                                             \
                                      exit (OPEN_FILE_ERROR);                                \
                                  }

#define _FCLOSE(addres) if(fclose(addres) != 0)                                              \
                        {                                                                    \
                            printf("ERROR CLOSE FILE\n"                                      \
                                   "LINE = %d\n", __LINE__);                                 \
                            exit(0);                                                         \
                        }                                                                    \


#endif // _ASSERT_H_
