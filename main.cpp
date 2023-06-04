#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <cctype>

class SystemConstant {
	public:
		static const std::string ID_KEYWORD;
		static constexpr int ID_NUMBER_LENGTH = 3;
};

const std::string SystemConstant::ID_KEYWORD = "SV";

// STRING UTILS
class StringUtils {
	public:
		static std::string NormalizeDateTime(std::string dateTime);
};

std::string StringUtils::NormalizeDateTime(std::string dateTime) {
	std::stringstream ss(dateTime);
	
	std::string token;
	
	std::string res {};
	
	while (getline(ss, token, '/')) {
		int n = token.size();
		
		if (n <= 2) {
			if (n == 2) {
				res += token + "/";
			} else {
				res += "0" + token + "/";
			}
		} else {
			res += token;			
		}
	}
	
	return res;
}


class Student {
	private:
		std::string _id, _name, _dob, _sClass;
		float _gpa;
		
	public:
		Student();
		void FillInformation();
		void ShowInformation();
		void SetId(std::string id);		
		std::string GetId() const;
		std::string GetName() const;
		std::string GetDob() const;
		std::string GetClass() const;
};

Student::Student() {
	this->_id = this->_name = this->_dob = this->_sClass = {};
	this->_gpa = {};
}

void Student::FillInformation() {
	getline(std::cin, this->_name);
	
	std::cin >> this->_sClass >> this->_dob >> this->_gpa;
	std::cin.ignore();
	
	this->_dob = StringUtils::NormalizeDateTime(this->_dob);
}

void Student::ShowInformation() {
	std::cout << this->_id << ' ' << this->_name << ' ' << this->_sClass << ' ' 
			  << this->_dob << ' ' << std::fixed << std::setprecision(2) << this->_gpa << std::endl;
}

void Student::SetId(std::string id) {
	this->_id = id;
}

std::string Student::GetId() const {
	return this->_id;
}

std::string Student::GetName() const {
	return this->_name;
}

std::string Student::GetDob() const {
	return this->_dob;
}

std::string Student::GetClass() const {
	return this->_sClass;
}




// STUDENT REPOSITORY
class StudentRepository {
	private:
		std::map<std::string, Student*> _mapIdToStudent;
	
	public:
		StudentRepository();
		~StudentRepository();
		void InsertStudent(std::pair<std::string, Student*> studentInformation);		
		int GetNumberOfStudents() const;
		std::vector<Student*> GetAll();
};

StudentRepository::StudentRepository() {	
}

StudentRepository::~StudentRepository() {
	for (auto it = this->_mapIdToStudent.begin(); it != this->_mapIdToStudent.end(); it++) {
		delete it->second;
	}
	this->_mapIdToStudent.clear();
}

void StudentRepository::InsertStudent(std::pair<std::string, Student*> studentInformation) {	
	this->_mapIdToStudent.insert(studentInformation);
}		
int StudentRepository::GetNumberOfStudents() const {

	return this->_mapIdToStudent.size();
}

std::vector<Student*> StudentRepository::GetAll() {
	std::vector<std::pair<std::string, Student*>> pStd(this->_mapIdToStudent.begin(), this->_mapIdToStudent.end());
	
	std::vector<Student*> vStd;

	for (const auto& student : pStd) {
		vStd.push_back(student.second);
	}

	return vStd;
}

//  STUDENT SERVICE
class StudentService {
	private:
		StudentRepository* _studentRepository;
	
	public:
		StudentService();
		StudentService(StudentRepository* studentRepository);
		~StudentService();
		void Add(Student* studentInformation);
		std::vector<Student*> GetAll();

	private:
		std::string MakeId();		
};

StudentService::StudentService() {	
	this->_studentRepository = nullptr;
}

StudentService::StudentService(StudentRepository* studentRepository) {
	this->_studentRepository = studentRepository;	
}

StudentService::~StudentService() {
	delete this->_studentRepository;	
}

void StudentService::Add(Student* studentInformation) {
	std::string idStd = this->MakeId();
	studentInformation->SetId(idStd);
	
	this->_studentRepository->InsertStudent(std::make_pair(idStd, studentInformation));
}

std::vector<Student*> StudentService::GetAll() {
	return this->_studentRepository->GetAll();
}

std::string StudentService::MakeId() {
	std::string res {};
	
	int nStds = this->_studentRepository->GetNumberOfStudents();
	
	++nStds;
	
	res = SystemConstant::ID_KEYWORD + 
		  std::string(SystemConstant::ID_NUMBER_LENGTH - std::to_string(nStds).size(), '0') +
		  std::to_string(nStds);
	
	return res;
}

// STUDENT CONTROLLER
class StudentController {
	
	private:
		StudentService* _studentService;
	
	public:
		StudentController();
		StudentController(StudentService* studentService);
		~StudentController();
		void AddNewStudent(Student* student);
		std::vector<Student*> GetAll();
};

StudentController::StudentController() {}
StudentController::StudentController(StudentService* studentService) {
	this->_studentService = studentService;
}
StudentController::~StudentController() {
	delete this->_studentService;
}

void StudentController::AddNewStudent(Student* student) {
	this->_studentService->Add(student);
}

std::vector<Student*> StudentController::GetAll() {
	return this->_studentService->GetAll();
}


// STUDENT VIEW
class StudentView {
	
	private:
		StudentController* _studentController;
	
	public:			
		StudentView();
		StudentView(StudentController* studentController);
		~StudentView();
		void ShowAllStudents();	
		void InitView();	
};

StudentView::StudentView() {
	this->_studentController = nullptr;
}
StudentView::StudentView(StudentController* studentController) {
	this->_studentController = studentController;
}

StudentView::~StudentView() {
	delete this->_studentController;
}

void StudentView::ShowAllStudents() {
	for (const auto& student : this->_studentController->GetAll()) {
		student->ShowInformation();
	}
}

void StudentView::InitView() {
	int n; std::cin >> n;
	std::cin.ignore();
	
	while (n--) {
		Student* student = new Student();
		student->FillInformation();
		
		this->_studentController->AddNewStudent(student);
	}
	
	this->ShowAllStudents();
}

int main() {
	
	StudentRepository* studentRepository = new StudentRepository();
	StudentService* studentService = new StudentService(studentRepository);
	StudentController* studentController = new StudentController(studentService);
	
	StudentView stdView(studentController);
	
	stdView.InitView();
	
	return 0;
}