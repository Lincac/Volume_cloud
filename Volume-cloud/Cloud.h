#pragma once

#include"ComputeShader.h"
#include"GenerateTexture.h"
#include"Camera.h"

class Cloud {
public:
	Cloud(int width, int height,float near,float far);
	~Cloud();

	void render(Camera &cam);

	void setSpeed(float newSpeed);
	void setCoverage(float newCoverage);
	void setCrispiness(float newCrispiness);
	void setCurliness(float newCurliness);
	void setDensity(float newDensity);
	void setAbsorption(float newAbsorption);
	void setType(float newType);

	void setcloudColorTop(glm::vec3 newcloudColorTop);
	void setcloudColorBottom(glm::vec3 newcloudColorBottom);
	void setLightColor(glm::vec3 newLightColor);

	unsigned int getCloudTexture() { return cloudMap; };
private:
	ComputeShader perlin_worley, worley, weather,RayMarch;

	int width, height;
	unsigned int perlin_worleyMap, worleyMap, weatherMap,cloudMap;

	float near, far;

	float speed;
	float coverage;
	float crispiness; 
	float curliness; 
	float density;
	float absorption; 
	float type;

	glm::vec3 cloudColorTop;
	glm::vec3 cloudColorBottom;

	glm::vec3 skyColorTop;
	glm::vec3 skyColorBottom;

	glm::vec3 lightCol;

	glm::mat4 projection;
	glm::mat4 view;

	void initShaders();
	void generateTexture();
};