#include <iostream>
#include <iomanip>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <functional>
#include <regex>
#include <set>


#include "nlohmann/json.hpp"
#include "yaml-cpp/yaml.h"

#include "proxy/proxy.h"

using namespace std;
using namespace nlohmann;

constexpr static string_view TEMPLATE_PATH = "template.json";
constexpr static string_view CLASH_PATH = "clash.yaml";
constexpr static string_view OUTPUT_PATH = "config.json";
constexpr static string_view CLOUDFLARE_DNS_RECORD_PATH = "cf_dns_record.txt";
constexpr static bool ENABLE_GENERATE_CLOUDFLARE_PROXY = true;
constexpr static bool ENABLE_GENERATE_CLOUDFLARE_DNS_RECORD = true;

void jsonTest()
{
    ifstream input(TEMPLATE_PATH.data());
    json template_file = json::parse(input);
    cout << setw(4) << template_file << endl;
    ofstream output(TEMPLATE_PATH.data());
    output << setw(4) << template_file;
}

void yamlTest()
{
    YAML::Node file = YAML::LoadFile(CLASH_PATH.data());
    cout << file;
}

Vmess generateCloudFlareProxy(const Vmess& proxy)
{
    constexpr static string_view CF_SERVER = "cf.jaychan.site";
    constexpr static string_view DOMAIN = "jaychan.site";
    constexpr static string_view CF_TAG = "(myself cf)";
    constexpr hash<string> hasher{};

    Vmess result = proxy;
    result.tag_ += CF_TAG;
    string host = to_string(hasher(result.server_));
    host += ".";
    host += DOMAIN;
    result.transport_->headers_.host_ = move(host);
    result.server_ = CF_SERVER;
    return result;
}

string generateCloudFlareDnsRecord(const string_view name, const string_view content)
{
    enum class Type
    {
        A,
        AAAA,
        CNAME
    };

    const static regex ipv4_regex("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
    const static regex ipv6_regex("((([0-9a-fA-F]){1,4})\\:){7}([0-9a-fA-F]){1,4}");
    static Type type;

    string result = name.data();
    result += ".\t1\tIN\t";
    if (regex_match(content.data(), ipv4_regex))
    {
        type = Type::A;
        result += "A";
    }
    else if (regex_match(content.data(), ipv6_regex))
    {
        type = Type::AAAA;
        result += "AAAA";
    }
    else
    {
        type = Type::CNAME;
        result += "CNAME";
    }
    result.append("\t").append(content);
    if (type == Type::CNAME)
    {
        result.append(".");
    }
    return result;
}

int main()
{
    YAML::Node clash_file = YAML::LoadFile(CLASH_PATH.data());
    auto proxies = clash_file["proxies"];
    json config = json::parse(ifstream(TEMPLATE_PATH.data()));
    auto& outbounds = config["outbounds"];
    auto& selector_outbounds = find_if(outbounds.begin(), outbounds.end(),
        [](const json& j) {
            if (j.is_object() && j.contains("type") && j.at("type") == "selector")
                return true;
            else
                return false;
        })->at("outbounds");
    set<string> cloud_flare_dns_record_set;
    for (const auto& proxy : proxies)
    {
        Vmess v;
        v.tag_ = proxy["name"].as<string>();
        v.server_ = proxy["server"].as<string>();
        v.server_port_ = proxy["port"].as<unsigned int>();
        v.uuid_ = proxy["uuid"].as<string>();
        v.security_ = proxy["cipher"].as<string>();
        v.alter_id_ = proxy["alterId"].as<int>();
        if (proxy["udp"].as<string>() == "false")
            v.network_ = "tcp";
        if (proxy["network"].as<string>() == "ws")
        {
            V2rayTransport ws;
            ws.path_ = proxy["ws-opts"]["path"].as<string>();
            Headers headers;
            headers.host_ = proxy["ws-opts"]["headers"]["Host"].as<string>();
            ws.headers_ = move(headers);
            v.transport_ = make_unique<decltype(ws)>(move(ws));
        }
        outbounds.push_back(v);
        selector_outbounds.push_back(v.tag_);
        auto cf_proxy = generateCloudFlareProxy(v);
        outbounds.push_back(cf_proxy);
        selector_outbounds.push_back(cf_proxy.tag_);
        if (ENABLE_GENERATE_CLOUDFLARE_DNS_RECORD)
            cloud_flare_dns_record_set.insert(generateCloudFlareDnsRecord(cf_proxy.transport_->headers_.host_, v.server_));
    }
    ofstream output(OUTPUT_PATH.data());
    output << setw(1) << setfill('\t') << config;
    if (ENABLE_GENERATE_CLOUDFLARE_DNS_RECORD)
    {
        output.close();
        output.open("cloud_flare_dns_record.txt", ios::trunc);
        copy(cloud_flare_dns_record_set.begin(), cloud_flare_dns_record_set.end(),
            ostream_iterator<string>(output, "\n"));
    }
}