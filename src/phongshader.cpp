#include "phongshader.h"

#include "resource_loader.h"

#define VERTEX_SHADER_FILE "shaders/phongVertex.vs"
#define FRAGMENT_SHADER_FILE "shaders/phongFragment.fs"

namespace ogle {

PhongShader *PhongShader::INSTANCE;
PointLight PhongShader::pointLights[MAX_POINT_LIGHTS];
int PhongShader::numPointLights = 0;

PhongShader *PhongShader::getInstance() {
    if (INSTANCE == nullptr) {
        INSTANCE = new PhongShader();
    }
    return INSTANCE;
}

PhongShader::PhongShader()
{
    std::string vertexShader = *ResourceLoader::loadShader(VERTEX_SHADER_FILE);
    std::string fragmentShader = *ResourceLoader::loadShader(FRAGMENT_SHADER_FILE);

    addVertexShader(vertexShader);
    addFragmentShader(fragmentShader);

    compileShader();

    addUniform("transform");
    addUniform("transformProjected");
    addUniform("baseColor");
    addUniform("ambientLight");

    addUniform("directionalLight.base.color");
    addUniform("directionalLight.base.intensity");
    addUniform("directionalLight.direction");

    addUniform("specularPower");
    addUniform("specularIntensity");
    addUniform("eyePos");

    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        addUniform("pointLights[" + std::to_string(i) + "].base.color");
        addUniform("pointLights[" + std::to_string(i) + "].base.intensity");
        addUniform("pointLights[" + std::to_string(i) + "].atten.constant");
        addUniform("pointLights[" + std::to_string(i) + "].atten.linear");
        addUniform("pointLights[" + std::to_string(i) + "].atten.exponent");
        addUniform("pointLights[" + std::to_string(i) + "].position");
    }

    BaseLight baseLight;
    Vector3f color = Vector3f(1.0f, 1.0f, 1.0f);
    baseLight.setColor(color);
    baseLight.setIntensity(0.0f);
    Vector3f direction = Vector3f(0.0f, 0.0f, 0.0f);
    m_directionalLight.setDirection(direction);
    m_directionalLight.setBase(baseLight);
}

void PhongShader::updateUniforms(Matrix4f& worldMatrix, Matrix4f& projectedMatrix, Mesh& mesh, Transform& transform) {
    Shader::setUniform("transformProjected", projectedMatrix);
    Shader::setUniform("transform", worldMatrix);
    Shader::setUniform("baseColor", mesh.getMaterial().getColor());
    Shader::setUniformf("specularPower", mesh.getMaterial().getSpecularPower());
    Shader::setUniformf("specularIntensity", mesh.getMaterial().getSpecularIntensity());
    Shader::setUniform("ambientLight", getAmbientLight());
    setUniform("directionalLight", getDirectionalLight());
    Shader::setUniform("eyePos", transform.getCamera().getPos());
    for (int i = 0; i < PhongShader::numPointLights; i++) {
        setUniform("pointLights[" + std::to_string(i) + "]", PhongShader::pointLights[i]);
    }

}

void PhongShader::setUniform(const std::string& uniformName, const DirectionalLight& directionalLight) {
    setUniform(uniformName + ".base", directionalLight.getBase());
    Shader::setUniform(uniformName + ".direction", directionalLight.getDirection());
}

void PhongShader::setUniform(const std::string& uniformName, const PointLight& pointLight) {
    setUniform(uniformName + ".base", pointLight.getBaseLight());
    setUniform(uniformName + ".atten", pointLight.getAtten());
    Shader::setUniform(uniformName + ".position", pointLight.getPosition());
}

void PhongShader::setUniform(const std::string& uniformName, const Attenuation& atten) {
    Shader::setUniformf(uniformName + ".constant", atten.getConstant());
    Shader::setUniformf(uniformName + ".linear", atten.getLinear());
    Shader::setUniformf(uniformName + ".exponent", atten.getExponent());
}

void PhongShader::setUniform(const std::string& uniformName, const BaseLight& baseLight) {
    Shader::setUniform(uniformName + ".color", baseLight.getColor());
    Shader::setUniformf(uniformName + ".intensity", baseLight.getIntensity());
}

void PhongShader::addPointLight(const PointLight& pointLight) {
    if (PhongShader::numPointLights == MAX_POINT_LIGHTS) {
        std::cerr << "Unable to add PointLight. Currently to many point lights for this scene" << std::endl;
        exit(1);
    }
    PhongShader::pointLights[PhongShader::numPointLights] = pointLight;
    PhongShader::numPointLights++;
}

PhongShader::~PhongShader()
{
    //TODO
}

}
