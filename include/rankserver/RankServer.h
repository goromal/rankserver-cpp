#pragma once

#include <filesystem>
#include <sorting/Sorting.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

class RankServer
{
public:
    RankServer(const std::string& data_dir);

    void load();
    void save();
    void resetState();
    void submitChoice(const sorting::ComparatorResult& choice);

    bool                     getErr() const;
    bool                     getDone() const;
    std::string              getMsg() const;
    std::vector<std::string> getRankList() const;
    std::string              getLeftItem() const;
    std::string              getRightItem() const;

private:
    bool                    mLoaded{false};
    std::string             mDataDir;
    sorting::QuickSortState mSortState;

    std::string              mLogFilename;
    std::string              mMapFilename;
    std::vector<std::string> mFileMap;
    std::vector<std::string> mRevRankList;

    bool                     mErr{false};
    std::string              mMsg;
    std::vector<std::string> mRankList;
};
