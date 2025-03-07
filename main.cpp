/**
* @file     	main.cpp
* @author   	huhh
* @email   	2234238825@qq.com
* @version	V1.0
* @date    	24-NOV-2014
* @license  	GNU General Public License (GPL)
* @brief   	Universal Synchronous/Asynchronous Receiver/Transmitter
* @detail		detail
* @attention
*  This file is part of OST.                                                    \n
*  This program is free software; you can redistribute it and/or modify 		\n
*  it under the terms of the GNU General Public License version 3 as 		    \n
*  published by the Free Software Foundation.                               	\n
*  You should have received a copy of the GNU General Public License   		    \n
*  along with OST. If not, see <http://www.gnu.org/licenses/>.       			\n
*  Unless required by applicable law or agreed to in writing, software       	\n
*  distributed under the License is distributed on an "AS IS" BASIS,         	\n
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  	\n
*  See the License for the specific language governing permissions and     	    \n
*  limitations under the License.   											\n
*   																			\n
* @htmlonly
* <span style="font-weight: bold">History</span>
* @endhtmlonly
* Version|Auther|Date|Describe
* ------|----|------|--------
* V3.3|Jones Lee|07-DEC-2017|Create File
* <h2><center>&copy;COPYRIGHT 2017 WELLCASA All Rights Reserved.</center></h2>
*/

#include <iostream>
#include "rpcServer.h"
#include "test.h"
#include "queueManager.h"
#include <type_traits>
using namespace std;
#include <tuple>

// 示例结构体
struct Person {
    std::string name;
    int age;
    double height;
    vector<string> dd;
    CONVJSON_INIT(name,age,height,dd);
};

void fwd()
{
    Person p;
    Json::Value root;
    queueManager qq;
    string mapName = "huhh";
    string iKey = "132";
    string iValue = "123123123";
    for(int i = 0;i<5;i++)
    {
        iKey += "a";
        qq.map_put(mapName,iKey,iValue);
    }
    cout<<"***************************"<<endl;

    cout<<qq.map_get(mapName,"123a")<<endl;
    //qq.map_destroy(mapName);
    cout<<qq.map_get_all(mapName)<<endl;
    cout<<qq.map_get_list(mapName,9,9)<<endl;

}

int main()
{
    std::string jsonString = R"({"name": "Alice", "age": 25, "height": 1.75,"dd":["12huhh3","123"]})";
    Person p ;

    Json::Value root;
    String2Json(root,jsonString);
    root["dd"][0].asString();
    Json2Struct(p,root);
    cout<<p.dd[0]<<endl;
    return 0 ;
    rpcServer rServer;
    rServer.StartRpcServer();
    return 0;
}