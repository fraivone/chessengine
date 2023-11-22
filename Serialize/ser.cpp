
#include <fstream>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/array.hpp>
#include "def.hpp"
#include "lut.hpp"

/////////////////////////////////////////////////////////////

int main() {
    // create and open a character archive for output
    {
    std::array<uint64_t,64>knight_lut = knight_position_lut();
    std::array<uint64_t,64>wpawn_fw_lut = wpawn_straight_lut();
    std::array<uint64_t,64>wpawn_doublefw_lut = wpawn_doublestraight_lut();
    std::array<uint64_t,64>wpawn_cap_lut = wpawn_diagcapture_lut();
    std::array<uint64_t,64>bpawn_fw_lut = bpawn_straight_lut();
    std::array<uint64_t,64>bpawn_doublefw_lut = bpawn_doublestraight_lut();
    std::array<uint64_t,64>bpawn_cap_lut = bpawn_diagcapture_lut();
    std::array<uint64_t,64>king_lut =  king_position_lut();
    std::unordered_map<std::string, std::array<uint64_t,64>> map_luts{
        {"knight_lut", knight_position_lut()},
        {"wpawn_fw_lut", wpawn_straight_lut()},
        {"wpawn_doublefw_lut", wpawn_doublestraight_lut()},
        {"wpawn_cap_lut", wpawn_diagcapture_lut()},
        {"bpawn_fw_lut", bpawn_straight_lut()},
        {"bpawn_doublefw_lut", bpawn_doublestraight_lut()},
        {"bpawn_cap_lut", bpawn_diagcapture_lut()},
        {"king_lut",  king_position_lut()},
    };
    
    int nRows = 64;
    int nCols = 4058;
    std::vector<std::vector<uint64_t>> fog(nRows, std::vector<uint64_t> (nCols, 0));

    for (auto const& x : map_luts){
        std::string name = x.first;
        std::array<uint64_t,64> lut = x.second;
        // save
        {
        std::ofstream ofs(name);
        boost::archive::text_oarchive oa(ofs);
        oa << boost::serialization::make_array(lut.data(), lut.size());
        
        }
        // load
        
        {
            std::array<uint64_t,64>  output;
            // create and open an archive for input
            std::ifstream ifs(name);
            boost::archive::text_iarchive ia(ifs);
            ia >> boost::serialization::make_array(output.data(), output.size());
            // archive and stream closed when destructors are called
        }
    }
    {
        std::ofstream ofs( "store.dat" );
        boost::archive::text_oarchive ar(ofs);
        ar & fog;
    }
    }
    

    // save data to archive
    {
        // boost::archive::text_oarchive oa(ofs);
        // write class instance to archive
        // oa << g;
    	// archive and stream closed when destructors are called
    }

    // ... some time later restore the class instance to its orginal state
    // gps_position newg;
    {
        // create and open an archive for input
        // std::ifstream ifs("filename");
        // boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        // ia >> newg;
        // archive and stream closed when destructors are called
    }
    return 0;
}