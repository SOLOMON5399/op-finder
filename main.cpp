#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <opencv2/opencv.hpp>
#include<map>
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <iomanip>
#include "OperationFinder.hpp"
#include "OperationStorage.hpp"
#include "OperationFinderAstAction.hpp"
#include "OperatorCounter.hpp"

using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;

cl::opt<std::string> GatesFile(
    "gates-file",
    cl::desc("Specify path to gates.json lookup table for gate count analysis"),
    cl::value_desc("filename"),
    cl::init("gates.json")
);

static cl::OptionCategory MyToolCategory("op-finder options");
static cl::opt<std::string> OutputFile("o", cl::desc("File to output the JSON to."), cl::cat(MyToolCategory));
static cl::opt<std::string> RootDirectory("r", cl::desc("The root directory of the source files."), cl::cat(MyToolCategory));
static cl::opt<bool> PrettyPrint("pretty", cl::desc("Pretty-print the output JSON."));
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

static cl::extrahelp MoreHelp("\nThe program takes the input <source0> ... files, parses their\n"
                              "AST and outputs a singular file containing a list of all noteworthy operations\n"
                              "for later analysis.\n");


#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

void visualizeCounts(const std::map<std::string, int>& counts,
                     const std::string& filename)
{
    // ---- Convert map → vector ----
    std::vector<std::pair<std::string,int>> vec(counts.begin(), counts.end());

    // ---- Sort descending ----
    std::sort(vec.begin(), vec.end(),
              [](const auto &a, const auto &b) {
                  return a.second > b.second;
              });

    // ---- Canvas settings ----
    int width = 1200;
    int height = 700;
    int margin = 100;

    cv::Mat img(height, width, CV_8UC3, cv::Scalar(255,255,255));

    // ---- Find max value ----
    int maxVal = 1;
    for (auto &p : vec)
        maxVal = std::max(maxVal, p.second);

    int barWidth = (width - 2 * margin) / vec.size();

    // ---- Draw axes ----
    cv::line(img, cv::Point(margin, height - margin),
                  cv::Point(width - margin, height - margin),
                  cv::Scalar(0,0,0), 2);

    cv::line(img, cv::Point(margin, margin),
                  cv::Point(margin, height - margin),
                  cv::Scalar(0,0,0), 2);

    // ---- Draw bars ----
    for (int i = 0; i < vec.size(); i++)
    {
        int val = vec[i].second;

        int barHeight = (val * (height - 2 * margin)) / maxVal;

        int x1 = margin + i * barWidth + 10;
        int y1 = height - margin - barHeight;
        int x2 = x1 + barWidth - 20;
        int y2 = height - margin;

        // Blue bar
        cv::rectangle(img, cv::Point(x1,y1), cv::Point(x2,y2),
                      cv::Scalar(255,0,0), cv::FILLED);

        // Value text (red)
        cv::putText(img, std::to_string(val),
                    cv::Point(x1, y1 - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(0,0,255), 2);

        // Operator label (X-axis)
        cv::putText(img, vec[i].first,
                    cv::Point(x1, height - margin + 25),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(0,0,0), 1);
    }

    // ---- Title ----
    std::string title = "Operator Usage - " + filename;
    cv::putText(img, title,
                cv::Point(width/4, 40),
                cv::FONT_HERSHEY_SIMPLEX, 1,
                cv::Scalar(0,0,0), 2);
    // ---- Axis Labels ----
    cv::putText(img, "Operators",
                cv::Point(width/2 - 50, height - 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7,
                cv::Scalar(0,0,0), 2);

    cv::putText(img, "Count",
                cv::Point(20, height/2),
                cv::FONT_HERSHEY_SIMPLEX, 0.7,
                cv::Scalar(0,0,0), 2);

    // ---- Save image ----
    std::string outFile = filename + "_graph.png";
    cv::imwrite(outFile, img);

    // ---- Show ----
    cv::imshow("Operator Usage Graph", img);
    cv::waitKey(0);
}

int main(int argc, const char** argv)
{
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory, llvm::cl::ZeroOrMore, "Usage: op-finder <options> <files...>");

    if (!ExpectedParser)
    {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    cv::Mat img = cv::imread("test.jpg");

    if (img.empty()) {
      llvm::outs() << "Image not found\n";
      }else {
    llvm::outs() << "Image loaded successfully\n";
      }


    CommonOptionsParser& OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    OperatorCounter op_counter;
    OperationStorage storage(&op_counter);

    if (PrettyPrint.getValue())
        storage.enablePrettyPrint();

    OperationFinder op_finder(&storage, &op_counter);

    if (!GatesFile.empty()) {
        std::ifstream gates_ifs(GatesFile.getValue());
        if (!gates_ifs.is_open()) {
            std::cerr << "Error: Could not open gates file: " << GatesFile.getValue() << "\n";
            return 1;
        }
        try {
            nlohmann::json gates_json = nlohmann::json::parse(gates_ifs);
            op_finder.loadGateCounts(gates_json);
            op_counter.loadGateCounts(gates_json);
        }
        catch (const nlohmann::json::parse_error& e) {
            std::cerr << "Error parsing gates file '" << GatesFile.getValue() << "': " << e.what() << "\n";
            return 1;
        }
    }

    OperationFinderAstAction action(&op_finder, &op_counter);
    Tool.run(newFrontendActionFactory(&action).get());
   auto counts = op_counter.getCombinedOperatorCounts();
   for (auto &p : counts)
   {
     std::cout << p.first << " : " << p.second << std::endl;
   }
   std::string filename = argv[1];
   filename = filename.substr(filename.find_last_of("/\\") + 1);

   visualizeCounts(counts, filename);
   
    const auto& user_defined_function_ops = op_finder.getUserDefinedFunctionOperators();    
    op_counter.scaleUserDefinedFunctionOperators(user_defined_function_ops);

    op_counter.printBasicOpGateBreakdown(std::cout);
    long long total_estimated_gates_from_basic_ops = op_counter.getTotalGateCount();    

    long long total_gates_from_system_calls = 0;
    const auto& function_call_counts = op_counter.getFunctionCallCounts();
    const auto& gate_counts = op_finder.getGateCounts();
    
    std::cout << "\n--- System Call Gate Analysis ---\n";
    for (const auto& pair : function_call_counts) {
        const std::string function_name = pair.first().str();
        long long call_count = pair.second;

        auto it = gate_counts.find(function_name);
        if (it != gate_counts.end()) {
            long long cost = it->second;
            long long function_gates = cost * call_count;
            total_gates_from_system_calls += function_gates;
            
            std::cout << "    " << std::left << std::setw(15) << function_name << ": "<< std::setw(5) << call_count << " calls * "
                      << std::setw(5) << cost << " gates/call = "<< function_gates << " gates\n";
        }
    }

    std::cout << "Total gates from system calls: " << total_gates_from_system_calls << "\n";
    std::cout << "--------------------------------\n";

    long long total_combined_gates = total_estimated_gates_from_basic_ops + total_gates_from_system_calls;
    std::cout << "\n--- Combined Gate Count ---\n";
    std::cout << "Total estimated gates (Basic Ops + System Calls): " << total_combined_gates << "\n";
    std::cout << "\n";
    
    if (!OutputFile.getValue().empty())
        storage.toFile(OutputFile.getValue());
    else
        storage.toStream(std::cout);

    op_counter.printCounts(std::cout);

    op_counter.printCounts(std::cout);
    op_counter.printBasicOpGateBreakdown(std::cout);
    
    long long total_gates_final = op_counter.getTotalGateCount();
  
    std::cout << "\nTotal estimated gates: "
          << total_estimated_gates_from_basic_ops <<
    std::endl;

    auto vis_data = op_counter.getCombinedOperatorCounts();

    visualizeCounts(vis_data, "output");

     return 0;
}
