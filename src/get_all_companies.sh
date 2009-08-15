{

wget http://www.nasdaqtrader.com/dynamic/SymDir/nasdaqlisted.txt 2>/dev/null &
rm -f 2>/dev/null stock[a-z].html
wget http://www.quotelinks.com/nyse/stocka.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockb.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockc.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockd.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stocke.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockf.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockg.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockh.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stocki.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockj.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockk.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockl.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockm.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockn.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stocko.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockp.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockq.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockr.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stocks.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockt.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stocku.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockv.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockw.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockx.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stocky.html 2>/dev/null &
wget http://www.quotelinks.com/nyse/stockz.html 2>/dev/null &
wait
}
{
snobol4 -b nasdaqlisted.sno <nasdaqlisted.txt
cat stock[a-z].html | snobol4 -b stock_nyse.sno 
} | sort -u >all_companies.dat
