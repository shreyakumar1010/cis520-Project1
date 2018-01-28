Adding alarm-mega
While adding the new test, I did the "grep -r alarmmultiple *" as suggested in the document to get an ides of what files I needed to chnge.
I also compared all the files in the src/tests/threads to see what needed to be changed.

I changed the following files:
1)"alarm-wait.c" had the same code as test_alarm_multiple but changing the number from 7 to 70
2)"test.c" adding "{"alarm-mega", test_alarm_mega}," to the program after test_single
3)"test.h" similar to the previous step
4)"Rubric.alarm" adding a line to test alarm-mega

After this I ran the "pintos -v -- run alarm-mega > log-mega.txt" to run the alarm-mega test and checked the output.
