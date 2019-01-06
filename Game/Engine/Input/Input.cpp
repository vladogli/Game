#include "Input.h"

Input::Input() {
	Reload();
}
void Input::Reload() {
	Clear();
	closeT = 0;
	for (size_t i = 0; i < 128; i++) {
		indicator[i] = KeyCombination::RELEASED;
	}
	myThread1 = new boost::thread(std::bind(&Input::FirstThreadFunc, this));
	myThread2 = new boost::thread(std::bind(&Input::SecondThreadFunc, this));
}
void Input::Clear() {
	while (combinations.size() > 0) {
		delete combinations[0];
		combinations.erase(combinations.begin());
	}
	closeT = 1;
	if(myThread1 != NULL){
		while (closeT <= 1) {
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
		closeT--;
		myThread1->interrupt();
		delete myThread1;
	}
	if(myThread2 != NULL) {
		while (closeT <= 1) {
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
		myThread2->interrupt();
		delete myThread2;
	}
}
Input::~Input() {
	Clear();
}
Input::KeyCombination::~KeyCombination() {
	delete values;
}
void Input::SecondThreadFunc() {
	unsigned char itr;
	while ( !closeT ) {
		for (itr = 0; itr < 128; itr++) {
			indicator[itr] = (GetKeyState(KeyMassive[itr].first) & 0x8000) ? 
					KeyCombination::HOLDING : KeyCombination::RELEASED;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds( 25 ));
	}
	closeT++;
}
void Input::FirstThreadFunc() {
	unsigned char itr;
	while ( !closeT ) {
		for (itr = 0; itr < combinations.size(); itr++) {
			bool _Value = 1;
			for (unsigned char i = 0; i < combinations[itr]->size; i++) {
				if (indicator[combinations[itr]->values[i]] != KeyCombination::HOLDING) {
					combinations[itr]->myState = KeyCombination::RELEASED;
					_Value = 0;
					combinations[itr]->Called = 0;
					break;
				}
			}
			if (_Value) {
				combinations[itr]->myState = KeyCombination::HOLDING;
				if (!combinations[itr]->Called) {
					combinations[itr]->Called = 1;
					boost::thread _T(combinations[itr]->callBackFuncPtr,std::ref(combinations[itr]->myState));
					_T.detach();
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds( 25 ));
	}
	closeT++;
}
void returnVal(std::vector<KEY>& keys, KEY k) {
	keys.push_back(k);
}
template<typename... Keys, typename = KEY>
void returnVal(std::vector<KEY>& keys, KEY first, Keys... k) {
	keys.push_back(first);
	returnVal(keys, k...);
}
template<typename... Keys, typename = KEY>
void Input::AddNewCombination(const std::function<void(Input::KeyCombination::state&)> function, Keys... args) {
	std::vector<KEY> keys;
	returnVal(keys, args...);
	AddNewCombination(keys, function);
}
void Input::AddNewCombination(const std::vector<KEY> keys, const  std::function<void(Input::KeyCombination::state&)> function) {
	KeyCombination *comb = new KeyCombination;
	comb->callBackFuncPtr = function;
	comb->size = (unsigned char)keys.size();
	comb->values = new KEY[keys.size()];
	for (unsigned char i = 0; i < keys.size(); i++) {
		for (unsigned char j = 0; j < 128; j++) {
			if (KeyMassive[j].first == keys[i]) {
				comb->values[i] = j;
				break;
			}
		}
	}
	combinations.push_back(comb);
}
std::string Input::GetValueName(const KEY _Value) const {
	for ( unsigned char i = 0; i < 128; i++ ) {
		if ( _Value == KeyMassive[i].first ) {
			return KeyMassive[i].second;
		}
	}
	return "";
}
