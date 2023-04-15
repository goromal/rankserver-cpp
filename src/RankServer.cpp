#include "rankserver/RankServer.h"

RankServer::RankServer(const std::string& data_dir) : mDataDir(data_dir) {}

void RankServer::load()
{
    using dir_iter = std::filesystem::recursive_directory_iterator;

    if (mErr)
    {
        return;
    }

    mFileMap.clear();
    mRankList.clear();
    mRevRankList.clear();

    struct stat info;
    if (stat(mDataDir.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR))
    {
        mErr = true;
        mMsg = "Data directory non-existent or broken: " + mDataDir;
        return;
    }
    std::vector<std::string> files;
    for (const auto& dirEntry : dir_iter(mDataDir))
    {
        std::string dirEntryStr = (std::stringstream() << dirEntry).str();
        std::string fileStr     = "";
        for (unsigned int i = dirEntryStr.size(); i > 0; i--)
        {
            char c = dirEntryStr[i - 1];
            if (c != '\"' && c != '/')
            {
                fileStr = c + fileStr;
            }
            if (c == '/')
            {
                break;
            }
        }
        if (fileStr.size() > 4 &&
            (fileStr.substr(fileStr.size() - 4) == ".txt" || fileStr.substr(fileStr.size() - 4) == ".png"))
        {
            files.push_back(fileStr);
        }
    }
    if (files.size() == 0)
    {
        mErr = true;
        mMsg = "Data directory has no rankable files (.txt|.png)";
        return;
    }
    mMapFilename = mDataDir + "/file_map.log";
    if (stat(mMapFilename.c_str(), &info) != 0)
    {
        mFileMap = files;
    }
    else
    {
        std::string   line;
        std::ifstream mapfile(mMapFilename);
        if (mapfile.is_open())
        {
            while (std::getline(mapfile, line))
            {
                if (line.size() > 4)
                {
                    mFileMap.push_back(line);
                }
            }
        }
        if (mFileMap.size() == 0)
        {
            mErr = true;
            mMsg = "Empty file map in provided data dir";
            return;
        }
    }
    mLogFilename = mDataDir + "/sort_state.log";
    if (stat(mLogFilename.c_str(), &info) != 0)
    {
        mSortState   = sorting::QuickSortState();
        mSortState.n = mFileMap.size();
        for (uint32_t i = 0; i < mSortState.n; i++)
        {
            mSortState.arr.push_back(i);
            mSortState.stack.push_back(0);
        }
        submitChoice(sorting::ComparatorResult::NOT_COMPARED);
    }
    else
    {
        auto [res, state] = sorting::sortStateFromDisk(mLogFilename);
        if (!res)
        {
            mErr = true;
            mMsg = "Sort state loading from file failed";
            return;
        }
        mSortState = state;
    }
    for (uint32_t i = 0; i < mSortState.arr.size(); i++)
    {
        mRankList.push_back(mFileMap[mSortState.arr[i]]);
        mRevRankList.push_back(mFileMap[mSortState.arr[mSortState.arr.size() - 1 - i]]);
    }
    mLoaded = true;
}

void RankServer::save()
{
    std::ofstream mapFile(mMapFilename);
    if (mapFile.is_open())
    {
        for (const auto& file : mFileMap)
        {
            mapFile << file << "\n";
        }
        mapFile.close();
    }
    if (!sorting::persistStateToDisk(mLogFilename, mSortState))
    {
        mErr = true;
        mMsg = "Failed to persist sort state to disk";
    }
}

void RankServer::resetState()
{
    sorting::QuickSortState resetState;
    resetState.n     = mSortState.n;
    resetState.arr   = mSortState.arr;
    resetState.stack = std::vector<uint32_t>(mSortState.n, 0);

    mSortState = resetState;
    submitChoice(sorting::ComparatorResult::NOT_COMPARED);
}

void RankServer::submitChoice(const sorting::ComparatorResult& choice)
{
    const static uint32_t maxIter  = 50;
    uint32_t              i        = 0;
    bool                  fullStep = false;

    mSortState.c = static_cast<uint32_t>(choice);

    while (!fullStep && i < maxIter)
    {
        auto [res, stateOut] = sorting::restfulQuickSort(mSortState);
        if (!res)
        {
            mErr = true;
            mMsg = "RESTful sort step failed";
            return;
        }
        mSortState = stateOut;
        if (mSortState.sorted == 1)
        {
            fullStep = true;
        }
        else if (mSortState.l == static_cast<uint32_t>(sorting::ComparatorLeft::I) && mSortState.p == mSortState.i)
        {
            mSortState.c = static_cast<uint32_t>(sorting::ComparatorResult::LEFT_EQUAL);
        }
        else if (mSortState.l == static_cast<uint32_t>(sorting::ComparatorLeft::J) && mSortState.p == mSortState.j)
        {
            mSortState.c = static_cast<uint32_t>(sorting::ComparatorResult::LEFT_EQUAL);
        }
        else
        {
            fullStep = true;
        }
        i += 1;
    }

    if (!fullStep)
    {
        mErr = true;
        mMsg = "RESTful sort timed out with incomplete steps";
        return;
    }
}

bool RankServer::getErr() const
{
    return mErr;
}

bool RankServer::getDone() const
{
    return mSortState.sorted == 1;
}

std::string RankServer::getMsg() const
{
    return mMsg;
}

std::vector<std::string> RankServer::getRankList() const
{
    return mRankList;
}

std::string RankServer::getLeftItem() const
{
    if (mLoaded && !mErr)
    {
        return mFileMap[mSortState.arr[mSortState.p]];
    }
    else
    {
        return "";
    }
}

std::string RankServer::getRightItem() const
{
    if (mLoaded && !mErr)
    {
        if (mSortState.l == static_cast<uint32_t>(sorting::ComparatorLeft::I))
        {
            return mFileMap[mSortState.arr[mSortState.i]];
        }
        else
        {
            return mFileMap[mSortState.arr[mSortState.j]];
        }
    }
    else
    {
        return "";
    }
}
