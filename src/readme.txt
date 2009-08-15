The status - it is a mess. I cnnot get correlat to run - it returns bad valuess for the numebr of seconds after a  while.

But I have a new idea- listen for the raindrops as they make waves,  but where in the pond do the raindrops lie?
use corr.dat as input - and make a predefined neural network - 2 layers deep.
Each layer has the same weights, thereby making this a non perceptron thingie.
The error rate is back propogated through twice which should error out the non causation items.
OR it might be a mess
see creator2.c for the beginning of this code.


correlat - broken
creator2 - reads companied.dat and corr.dat and produces the.net
archive_to_ticker.c - generates the input to feed the beast - not working yet.
normalize - convert absolute values to delta values - log
train - read normalize and train "the.net" neural network to be accurate. Works very well.


so 1. run correlat - which doesn't work.
2. use awk and sort -u to get a list of all correlated companies into companies.dat
3. run creator2
4. run archive_to_ticker to prepare the training data
5. train the fuck out of it.

creator2 places random the correlations which will make the initial 
machine super sensitive.  So training should be pretty rough in the beginning.
I can see it pegging to all 1 or all -1 pretty easily.
Perhaps the randomness shouldd include either direction - Iduknow.




nasdaq listed:
http://www.nasdaqtrader.com/dynamic/SymDir/nasdaqlisted.txt
wget http://www.nasdaqtrader.com/dynamic/SymDir/stocka.html



--------------------------------------------------


Shit.  It doesn't work for shit.
And i needed totake a break.
I got the fast GPU thing working -- effectively 10x faster with a normal input-- but it seems to do real badly with thousands of inputs.
And personal bullshit of someone else got in the fucking way.

So now I am thinking of protecting the node input/output -- it could be on each weight,  or it could be from neuron to neuron.  Kinda like
Michael Azoff called -- I forgot - It was an extra training node -- where all others are locked.
And then have the thing train very simple - one input one output - maybee 2 or 3 nodes and then lock them and add the 
three related nodes and then add the rest of the back -dated neurons. 
I did a normalization by dividing by 1000.  this caused the 128 monkey solution to guess within +-40 points.  It was ridiculously bad.  
So probably having the number around  .5 would be better. You would think it would be more stable.







V2:
1. Place all data used for correlation in data
2. run simplify >sample.csv


3. correlat
correlat.c - This prints out correlation of stocks where
the number is > 9? %. 
ai
This is used to segment into various groups - each group becomes a new
directory and a new set of agents to spawn.

To build an agent -  we build 100 random agents and take them up to 
500 nodes - lets do 2 layer right now.

Then we take the 10 brightest and dupe them

We want it to use all the computers in a cluster.


OK - so we run correlat | build_correlators
build_correlators will split the correlations into sets
And then build 100 random machines.
Then run trainer - trainer will make 
the input lines for the 100 random machines - each machine eats it and spits 
out an answer.  then the trainer will also do feedback lines where the
fields will get their training.

Machines take
input values
feed values outvalues
down


The output of the nnl machines  - there will be a context file
And the output stream
output values outvalues
accuracy values outaccuracy





V1:

hello
Hib - Jul 2006
I worked on this a year ago.

The first program is correlat.c -- this program looks for statistical correlation
And it prints out all of them.  It takes about 200M and 15 minutes to run.

Then prints out values with a correlation index >95.

It does some trics.
1.  It takes the logarithm of all values,  to reduce the difference
2.  It also divides over the average so that we wont correlate towards the average.
This works pretty well.  Take
RYN     UBA     0.977201
RYN     ANL     0.957877
RYN     AMC     1.010325
RYN     CMM     0.978489
RYN     BDWGP.PK        1.072963
All of these are real-estate mortgage companies. Go figure.






That was real cool. Then we went on to do 
correlat_bihourly.c 

This one looks back on a logarithmic scale -- so it is faster and uses less data points.
But once it is done,  It goes further to determine causation,  by shifting the fields and looking
for a correlation shifted slightly.

I thought this would be a precursor to DFT style correlation,  only I found out later
that I was basically doing a DFT correlation.  More on this later.



For good looking candidates -- where there are less thatn 10 related,  we look for bihourlies.
I think this one takes much longer to run.  After the initial computation,
doing correlation and shifting is o(n^2)  with many calls.


It does pretty good, though.

Example:
BRO leads PTIE by 11220 minutes -0.961853 -0.885972
FMT leads BMY by 14400 minutes -0.974153 -0.902677
causation.txt has them all listed.




I think that causation_from_correlation.c is a version of correlat_bihourly that was started
and not finished.









then simplify.c this converts the data from bihourly with all the variables
to jus thaving one variable -- the price.



make_money - this uses the simple.csv file -- quicker.
It reads causation.txt and looks at what is going on.  Based on that it tries to buy a
and sell to make money.
Well,  the first month,  we won alot.
The next months,  we lost a heck of alot.

So much for causation.











Sp that brings us to July,  and thanks to a shot of Desmopressin,  I am 
looking at neural networks.

I don't like it -- we have these neural networks,  and sigma and stuff.
It seems that the better non-linear to use instead of sigma would be
e^x. 

The idea here is to use these clusters of 6 stocks or so,  and get measurements
in the input.  And then on the output,  it should be equal to what the next measurement is.

It could be real nice.




So I am going to do correlat_into_nnl which will do the following:
1. it will take the groups of 6 and run make_money2_group on that group.
Then make_money_group will train 100 neural nets.  
Each neural net then will be run and continuouslty learning.
something will look at the neural nets output and for large jumps, 
decide to bet.  



Other ideas:
correlation -- to find common frequencies.  Actually, I guess convolution would 
be better.  Anyways,  if the frequencies are similar,  you might assume that
if the frequency is in one,  it might exist in the other.
Limit based on fundimental analysis.  Like look at stocks with a higher P/E ration,  or 
profit,  positive cash flow for a while.  By separating the wheat from the chaff,  perhaps we 
can get some seed.


