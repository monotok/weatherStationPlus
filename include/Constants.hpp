//
// Created by hammer on 23/03/2021.
//

#ifndef WEATHERSTATIONPLUS_CONSTANTS_HPP
#define WEATHERSTATIONPLUS_CONSTANTS_HPP

namespace LcdConstants
{
    const short ONE_DAY = 1;
    const short ONE_WEEK = 2;
    const short ONE_MONTH = 3;
    const short ONE_YEAR = 4;

    const std::map<short, string> timeframeText = {
            make_pair(ONE_DAY, "One Day"),
            make_pair(ONE_WEEK, "One Week"),
            make_pair(ONE_MONTH, "One Month"),
            make_pair(ONE_YEAR, "One Year"),
    };

    const short FIXED = 0;
    const short VAR = 1;

    const string CURRENT = "current";
    const string DATE = "date";
    const string HOMEPAGE = "homepage";
    const string TIMEFRAME = "tf";
}

#endif //WEATHERSTATIONPLUS_CONSTANTS_HPP
