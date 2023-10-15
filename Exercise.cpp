  /*
 * @Author: Jingyao Suo 
 * @Date: 2023-10-15 16:34:32
 * @Last Modified by: Jingyao Suo
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;


// Define a struct to represent a trade
struct Trade {
    long long TimeStamp;  // Microseconds since midnight
    string Symbol;        // Unique symbol
    int Quantity;         // Trade amount
    double Price;         // Trade price
};


// Read in vector of trades from file
vector<Trade> read_in_trades(string file_name) {

    vector<Trade> trades;
    // Open the input file
    ifstream infile(file_name);
    if (!infile)
    {
        throw "Error while loading input file!";
    }

    string line;
    while (getline(infile, line)) {
        Trade trade;
        size_t start = 0;
        size_t end = 0;

        // CSV format: TimeStamp,Symbol,Quantity,Price
        end = line.find(',', start);
        trade.TimeStamp = stoll(line.substr(start, end - start));
        start = end + 1;

        end = line.find(',', start);
        trade.Symbol = line.substr(start, end - start);
        start = end + 1;

        end = line.find(',', start);
        trade.Quantity = stoi(line.substr(start, end - start));
        start = end + 1;

        trade.Price = stod(line.substr(start));

        trades.push_back(trade);

        // cout << trade.TimeStamp << "," << trade.Symbol << "," << trade.Quantity << "," << trade.Price << endl;
    }
    
    // Close the file
    infile.close();

    return trades;
};


// Create a hashmap to store trade data for each symbol
map<string, vector<Trade>> read_symbolTrades(vector<Trade> trades)
{
    map<string, vector<Trade>> symbolTrades;

    // Populate the symbolTrades map
    for (const Trade& trade : trades) {
        symbolTrades[trade.Symbol].push_back(trade);
    }
    return symbolTrades;
};


// calculate different indicators and output to file
void output_inds(map<string, vector<Trade>> symbolTrades, string output_file="./output.csv")
{
    // Open the output file
    ofstream outputFile(output_file);
    if (!outputFile)
    {
        throw "Error while loading output file!";
    }

    // Write the header to the output file
    outputFile << "Symbol,MaxTimeGap,Volume,WeightedAveragePrice,MaxPrice" << endl;

    // Calculate and output the requested indicators for each symbol to the output file
    for (const auto& pair : symbolTrades) {
        const vector<Trade>& symbolTradeData = pair.second;

        long long maxTimeGap = 0;
        int totalVolume = 0;
        // actually total trade volume, normalization in the end
        double weightedAvgPrice = 0.0;
        double maxTradePrice = 0.0;

        for (size_t i = 0; i < symbolTradeData.size(); i++) {
            // update maxTimeGap
            if (i > 0) {
                long long timeGap = symbolTradeData[i].TimeStamp - symbolTradeData[i - 1].TimeStamp;
                maxTimeGap = max(maxTimeGap, timeGap);
            }

            // update totalVolume
            totalVolume += symbolTradeData[i].Quantity;

            // Update weightedAvgPrice
            weightedAvgPrice += symbolTradeData[i].Quantity * symbolTradeData[i].Price;

            // Update maxTradePrice
            maxTradePrice = max(maxTradePrice, symbolTradeData[i].Price);
        }

        // Normalize weightedAvgPrice
        if (totalVolume > 0) {
            weightedAvgPrice /= totalVolume;
        }
        // take lower integer

        // Write the results to the output file
        outputFile << pair.first << ","
                   << maxTimeGap << ","
                   << totalVolume << ","
                   << static_cast<int>(weightedAvgPrice) << ","
                   << maxTradePrice 
                   << endl;
    }

    // Close the output file
    outputFile.close();

};






int main()
{
    // read in csv and store in vector
    vector<Trade> trades = read_in_trades("./input.csv");

    // store in hashmap for each symbol
    map<string, vector<Trade>> symbolTrades = read_symbolTrades(trades);
    
    // calculate indicators and output
    output_inds(symbolTrades, "./output.csv");

    system("pause");
    return 0;
}