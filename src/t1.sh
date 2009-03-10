{
echo "output 0
learning_rate .001"
grep train natr.trn
grep train natr.trn
grep train natr.trn
grep train natr.trn
grep train natr.trn
grep train natr.trn
grep train natr.trn
grep train natr.trn
grep train natr.trn
grep train natr.trn

echo "output 1
learning_rate 0.000"
grep train natr.trn | head
} | ./ai NATR_stocks.dat 1  natr1.net natr1.net

