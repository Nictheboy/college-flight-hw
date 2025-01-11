#pragma once
#include <memory>
#include <miniSTL/stl.hpp>
#include <optional>
#include <string>
#include "flight_types.hpp"
#include "surakarta_logger.hpp"

class FlightDatabase {
   public:
    FlightDatabase(std::string filename, std::shared_ptr<SurakartaLogger> logger = std::make_shared<SurakartaLoggerNull>());
    struct Record {
        Key id;
        Airport airport_from, airport_to;
        DateTime datetime_from, datetime_to;
        Price price;
    };

    DateTime ParseDateTime(std::string datetime);
    Record QueryRecordById(Key id) const;
    std::shared_ptr<List<Key>> QueryRecordIdsByAirportFrom(Airport airport) const;
    std::shared_ptr<List<Key>> QueryRecordIdsByAirportTo(Airport airport) const;
    DateTime AirportMin() const { return airport_min; }
    DateTime AirportMax() const { return airport_max; }

   private:
    std::shared_ptr<SurakartaLogger> logger;

    Vector<Record> records;
    Record ParseRecord(std::string line);
    void LoadDatabase(std::string filename);

    Airport airport_min, airport_max;
    void InitAirportRange();

    Vector<std::shared_ptr<List<Key>>> airport_from_bucket_index, airport_to_bucket_index;
    void InitAirportBucketIndex();
};
