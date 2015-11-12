#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/QueryEnvironment.hpp"
#include "indri/Parameters.hpp"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>

vector<string> read_docnos_file(const std::string& input_file_path) {
  string line;
  ifstream f (input_file_path.c_str());
  vector<string> l;
  if (f.is_open())
  {
    while ( getline (f, line) )
    {
      l.push_back(line);
    }
    f.close();
  }
  return l;
}

void output_docs_text(const std::string& output_fn, const char* text) {
  ofstream f (output_fn.c_str());
  if (f.is_open())
  {
    f << text;
    f.close();
  }
  else cout << "Unable to open file:" << output_fn << endl;
}

void print_multiple_docs_text(indri::collection::Repository& r, const std::string& input_file_path, const std::string& output_dir) {
  vector<string> l = read_docnos_file(input_file_path);
  indri::collection::CompressedCollection* collection = r.collection();
  for (int i = 0; i != l.size(); i++) {
      std::string attributeName = "docno";
      std::string attributeValue = l[i];
      std::vector<lemur::api::DOCID_T> docids = collection->retrieveIDByMetadatum( attributeName, attributeValue );
      if (docids.size() > 0) {
          lemur::api::DOCID_T internal_docid = docids[0];
          indri::api::ParsedDocument* document = collection->retrieve( internal_docid );
          output_docs_text(output_dir+"/"+attributeValue, document->text);
          delete document;
      }
  }  
}

void usage() {
    std::cout << "    IndriDumpMultiDocs -index=... -docnofile=... [-outputdir=...]" << std::endl;
    std::cout << "    1. index - index path" << std::endl;
    std::cout << "    2. docnofile - input file that contains all the docno" << std::endl; 
    std::cout << "    3. outputdir - outputdir, optional" << std::endl;
}

void require_parameter( const char* name, indri::api::Parameters& p ) {
  if( !p.exists( name ) ) {
    usage();
    LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a " + name + " parameter." );
  }
}


int main( int argc, char** argv ) {
  try {
    indri::api::Parameters& parameters = indri::api::Parameters::instance();
    parameters.loadCommandLine( argc, argv );

    require_parameter( "index", parameters );
    require_parameter( "docnofile", parameters );

    indri::collection::Repository r;
    std::string repName = parameters["index"];
    std::string inputFile = parameters["docnofile"];
    std::string outputDir("dump_multidocs");

    if ( parameters.get( "outputdir", "" ) != "") {
      std::string outputRoot = parameters["outputdir"];
      outputDir = outputRoot+"/"+outputDir;
    }
    const int dir_err = system(("mkdir -p "+outputDir).c_str());
    if (-1 == dir_err)
    {
      printf("Error creating directory!n");
      exit(1);
    }
    
    r.openRead( repName );
    print_multiple_docs_text(r, inputFile, outputDir);
    r.close();
    return 0;
  } catch( lemur::api::Exception& e ) {
    LEMUR_ABORT(e);
  }
}


