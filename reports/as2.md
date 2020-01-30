# Assignment2 Report

## Result 
 
> produced value is: 5  
> produced value is: 6  
> produced valueconsumed value is: 7   
> consumed value is: 7   
> consumed value is: 7   
> consumed value is: 7   
> consumedxsh $  is: 7   
> produced value is: 8   
> produced value is: 9   
> produced value is: 10   
> produced value  value is: 7   
> consumed value is: 11   
> consumed value is: 11   
> consumed value is: 11   
> consumed value is: 11   
> produced value is: 12   
> produced value is: 13   
> produced value is: 14   
> produced value isis: 11   
> consumed value is: 15   
> consumed value is: 15   
> consumed value is: 15   
> consumed value is: 1: 15   
> produced value is: 16   
> produced value is: 17   
> produced value is: 18   
> produced value is: 19 5   
> consumed value is: 19   
> consumed value is: 19   
> consumed value is: 19   
> consumed value is: 19   
> con  
> produced value is: 20   
> sumed value is: 19   
> consumed value is: 20   
> consumed value is: 20   
> consumed value is: 20   

## Questions to Answer

### Does your program output any garbage? If yes, why?

The answer is **YES**. I believe it's because the OS is continuously switching between our *producer* and *consumer* programs, and even the *xsh* shell is involved. So the function call *printf* is sometimes interrupted just between switching to execute another program.

### Are all the produced values getting consumed? Check your program for a small count like 20.

Definately **NOT** all produced values are consumed. We can see that a lot of values are not consumed from the result.
