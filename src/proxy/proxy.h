#pragma once

#include <string>
#include <vector>
#include <memory>

#include "nlohmann/json.hpp"

using namespace std;
using namespace nlohmann;

class Headers
{
public:
    string host_;
};

class Tls {};

class V2rayTransport
{
public:
    constexpr static string_view type_ = "ws";
    string path_;
    Headers headers_;
    int max_early_data_ = 0;
    string early_data_header_name_ = "";
};

class Vmess
{
public:
    Vmess() = default;
    Vmess(const Vmess& another)
    {
        copy_from(another);
    }

public:
    constexpr static string_view type_ = "vmess";
    string tag_;
    string server_;
    unsigned int server_port_;
    string uuid_;
    string security_ = "auto";
    int alter_id_ = 0;
    bool global_padding_ = false;
    bool authenticated_length_ = false;
    string network_;
    Tls tls_;
    string packet_encoding_;
    unique_ptr<V2rayTransport> transport_;

private:
    void copy_from(const Vmess& another)
    {
        tag_ = another.tag_;
        server_ = another.server_;
        server_port_ = another.server_port_;
        uuid_ = another.uuid_;
        security_ = another.security_;
        alter_id_ = another.alter_id_;
        global_padding_ = another.global_padding_;
        authenticated_length_ = another.authenticated_length_;
        network_ = another.network_;
        tls_ = another.tls_;
        packet_encoding_ = another.packet_encoding_;
        transport_ = make_unique<remove_reference_t<decltype(*transport_)>>(*another.transport_);
    }
};

void to_json(json& j, const Headers& h);
void to_json(json& j, const Vmess& v);