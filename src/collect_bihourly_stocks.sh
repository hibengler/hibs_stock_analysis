#!/bin/bash
. /u/stocks/environment
cd data
d="sd"`date '+%Y%m%d'`".csv"
perl ../collect_bihourly_stocks.pl $1 >>$d
